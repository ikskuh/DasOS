#include <stdint.h>
#include <new>
#include <string.h>
#include "multiboot.hpp"
#include "pmm.hpp"
#include "vmm.hpp"
#include "console.hpp"
#include "elf.hpp"
#include "idt.hpp"
#include "gdt.hpp"
#include "asm.hpp"

#include "syscalls.h"
#include "keyboard.hpp"
#include "video.hpp"

VMMContext * kernelContext;

uint16_t *video = (uint16_t*)0xB8000;

uint32_t LOWMEM;
uint32_t HIGHMEM;

void initialize_pmm(multiboot::Structure const & mb);

void initialize_vmm();

void load_elf(multiboot::Module const & module);

void dasos_demo();

void timer_intr(CpuState * & cpu) { }

void secure_multiboot(multiboot::Structure const & mb, void (*fn)(uint32_t addr));

extern "C" void init(multiboot::Structure const & mb)
{
	using namespace console_tools;
	
	initialize_pmm(mb);
	
	secure_multiboot(mb, [](uint32_t a) { PMM::markUsed(physical_t(a)); });
	
	Console::main << "ptr:   " << hex((uint32_t)mb.vbe.modeInfo) << "\n";
	Console::main << "siz-x: " << (uint32_t)mb.vbe.modeInfo->res.x << "\n";
	Console::main << "siz-y: " << (uint32_t)mb.vbe.modeInfo->res.y << "\n";
	Console::main << "pitch: " << (uint32_t)mb.vbe.modeInfo->pitch << "\n";
	Console::main << "video: " << (uint32_t)mb.vbe.modeInfo->framebuffer << "\n";
	
	
	GDT::initialize();
	
	IDT::initialize();
	
	initialize_vmm();
	
	secure_multiboot(mb, [](uint32_t a) { 
		kernelContext->map(
			virtual_t(a),
			physical_t(a),
			VMMFlags::Writable);
	});
	
	Console::main << "Active Paging...\n";
	VMM::enable();

	vid_init(mb.vbe.modeInfo);

	Console::main
		<< "Flags:       " << bin(mb.flags) << "\n"
		<< "LowMem:      " << hex(LOWMEM) << "\n"
		<< "HighMem:     " << hex(HIGHMEM) << "\n"
		<< "Commandline: " << mb.commandline << "\n"
		<< "Bootloader:  " << mb.bootLoaderName << "\n"
		;
	
	IDT::interrupt(0x20) = Interrupt(timer_intr);
	kbd_init();
	
	ASM::sti();
	
	if(mb.modules.length == 0) {
		Console::main << "No multiboot modules found, starting demo...\n";
		dasos_demo();
		return;
	}
	
	multiboot::Module const & module = mb.modules[0];
	
	Console::main << "Loading module: " << module.name << "\n";
	
	load_elf(module);
	
	Console::main << "Starting program...\n";
	{
		typedef void (*mainfunction)(struct syscalls *syscalls);
		
		mainfunction main = (mainfunction)LOWMEM;
		main(&SYSCALLS);
	}
	Console::main << "Program finished.\n";
	
	while(1);
}

void secure_multiboot(multiboot::Structure const & mb, void (*fn)(uint32_t addr))
{
	auto caa = [](void const * p) -> uint32_t { return (uint32_t)p & 0xFFFFF000; };
	fn(caa(&mb));
	fn(caa(mb.commandline));
	fn(caa(mb.bootLoaderName));
	fn(caa(mb.vbe.modeInfo));
	
	for(multiboot::Module const & mod : mb.modules) {
		fn(caa(&mod));
		for(uint32_t s = caa(mod.start.data()); s < mod.end.numeric(); s += 0x1000) {
			fn(s);
		}
	}
	
	auto const & vbe = *mb.vbe.modeInfo;
	
	for(uint32_t i = 0; i < vbe.res.y * vbe.pitch; i += 0x1000) {
		fn(vbe.framebuffer + i);
	}
}

void load_elf(multiboot::Module const & module)
{
	elf::Header const &file = *module.start.data<elf::Header const>();
	if(file.magic != elf::MAGIC)
	{
		Console::main << "Invalid ELF magic.\n";
		return;
	}
	Console::main << "ELF Version: " << file.version << "\n";
	
	
	elf::ProgramHeader const *header  = (elf::ProgramHeader const *)(module.start.numeric() + file.ph_offset);
	
	for(uint32_t i = 0; i < file.ph_entry_count; i++, header++) {
	
		Console::main 
			<< "Program Header " << (uint32_t)i << ": " 
			<< header->type << ", "
			<< header->virt_addr << "@" << header->offset
			<< "\n";
		/* Nur Program Header vom Typ LOAD laden */
		if (header->type != 1) {
				continue;
		}
		
		if(header->virt_addr < LOWMEM) {
			Console::main << "ELF Section below LOWMEM, skipping...\n";
			continue;
		}
	
		void* dest = (void*)header->virt_addr;
		void* src = ((char*)&file) + header->offset;
		
		memset(dest, 0, header->mem_size);
		memcpy(dest, src, header->file_size);
	}
}

void initialize_vmm()
{
	kernelContext = new (PMM::alloc().data()) VMMContext();
	
	Console::main << "Mapping memory...\n";
	for(uint32_t addr = 0; addr < 4096 * 1024; addr += 0x1000) {
		kernelContext->map(
			virtual_t(addr), 
			physical_t(addr),
			VMMFlags::Writable | VMMFlags::UserSpace);
	}
	kernelContext->map(
		virtual_t(kernelContext),
		physical_t(kernelContext),
		VMMFlags::Writable);
		
	uint32_t userspace_size = 4096; // 16 MB?
	
	LOWMEM = 0x40000000;
	HIGHMEM = 0x40000000;
	//*
	for(uint32_t i = 0; i < userspace_size; i++) {
		
		kernelContext->map(
			virtual_t(HIGHMEM),
			PMM::alloc(),
			VMMFlags::Writable);
		
		HIGHMEM += 0x1000;
	}
	// */
		
	Console::main << "Active Context...\n";
	VMM::activate(*kernelContext);
}

struct marker;

extern marker kernelStartMarker;
extern marker kernelEndMarker;

void initialize_pmm(multiboot::Structure const & data)
{
	for(auto &mmap : data.memoryMaps) {
		if(mmap.length == 0) {
			continue;
    }
    if(mmap.isFree() == false) {
      continue;
    }
		
    if(mmap.base > 0xFFFFFFFF) {
        //Console::main << "mmap out of 4 gigabyte range." << "\n";
        continue;
    }    
    if(mmap.isFree()) {
      // Mark all free memory free...
      physical_t lower = physical_t(mmap.base).alignUpper(4096);
      physical_t upper = physical_t(mmap.base + mmap.length).alignLower(4096);
      
      uint32_t ptr = lower.numeric();
      while (ptr < upper.numeric()) {
        PMM::markFree(physical_t(ptr));
        ptr += 0x1000;
      }
    }
  }
  
  // Mark all memory used by the kernel used...
  physical_t lower = physical_t(&kernelStartMarker).alignLower(4096);
  physical_t upper = physical_t(&kernelEndMarker).alignUpper(4096);
  
  uint32_t ptr = lower.numeric();
  while (ptr < upper.numeric()) {
    PMM::markUsed(physical_t(ptr));
    ptr += 0x1000;
  }
	
	// nullptr is not valid.
	PMM::markUsed(physical_t(nullptr));
	
	// Mark the video memory as used.
	PMM::markUsed(physical_t(0xB8000));
}



extern "C" void _puts(const char *s)
{
	Console::main << s;
}

struct syscalls SYSCALLS = 
{
	&kbd_getchar,
	&kbd_is_pressed,
	&kbd_getkey,
	&video_clear,
	&video_buffer,
	&video_getmode,
	&_puts,
};