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
#include "timer.hpp"
#include "fs.h"

static const uint32_t USERSTART = 0x40000000;

typedef void (*mainfunction_t)(struct syscalls *syscalls);

VMMContext * kernelContext;

uint16_t *video = (uint16_t*)0xB8000;

uint32_t RAMEND;

void initialize_pmm(multiboot::Structure const & mb);

void initialize_vmm();

mainfunction_t load_elf(multiboot::Module const & module);

void dasos_demo();

void secure_multiboot(multiboot::Structure const & mb, void (*fn)(uint32_t addr));
		
extern "C" void init(multiboot::Structure const & mb)
{
	using namespace console_tools;
	
	initialize_pmm(mb);
	
	secure_multiboot(mb, [](uint32_t a) { PMM::markUsed(physical_t(a)); });
	
	GDT::initialize();
	
	IDT::initialize();
	
	initialize_vmm();
	
	secure_multiboot(mb, [](uint32_t a) { 
		kernelContext->map(
			virtual_t(a),
			physical_t(a),
			VMMFlags::Writable);
	});

	vid_init(mb.vbe.modeInfo);
	{ // Provide video backbuffer memory.
		videomode_t vmode;
		video_getmode(&vmode);
		
		size_t siz = (vmode.stride * vmode.height);
		for(uint32_t i = 0; i < siz; i += 0x1000) {
			uint32_t addr = (uint32_t)video_buffer() + i;
			kernelContext->map(
					virtual_t(addr),
					PMM::alloc(),
					VMMFlags::Writable);
		}
	}
	
	{ // Map available memory into 
		uint32_t freeMem = PMM::getFreeMemory();
		int32_t userspace_size = (freeMem >> 12) - 256; // 16 MB?
		
		Console::main << (freeMem >> 20) << " MB free RAM\n";
		Console::main << userspace_size << " pages for user space.\n";
		if(userspace_size <= 0) {
			Console::main << "Insufficient memory!\n";
			return;
		}
		
		RAMEND = USERSTART;
		for(int32_t i = 0; i < userspace_size; i++) {
			
			kernelContext->map(
				virtual_t(RAMEND),
				PMM::alloc(),
				VMMFlags::Writable);
			
			RAMEND += 0x1000;
		}
	}
	
	Console::main << "Active Context...\n";
	VMM::activate(*kernelContext);
	
	Console::main << "Active Paging...\n";
	VMM::enable();
	
	SYSCALLS.video_clear({255, 0, 255});

	Console::main
		<< "Flags:       " << bin(mb.flags) << "\n"
		<< "RamEnd:     " << hex(RAMEND) << "\n"
		<< "Commandline: " << mb.commandline << "\n"
		<< "Bootloader:  " << mb.bootLoaderName << "\n"
		;
	
	timer_init();
	kbd_init();
	
	ASM::sti();
	
	if(mb.modules.length == 0) {
		Console::main << "No multiboot modules found, starting demo...\n";
		dasos_demo();
		return;
	}
	
	multiboot::Module const & module = mb.modules[0];
	
	Console::main << "Loading module: " << module.name << "\n";
	
	mainfunction_t main = load_elf(module);
	
	if(((uint32_t)main) < USERSTART)
	{
		Console::main << "Could not find a fitting entry point :(\n";
		return;
	}
	
	Console::main << "Starting program...\n";
	{
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

static void blitIcon(void *ptr)
{
	uint8_t *image = (uint8_t*)ptr;
	for(uint32_t y = 0; y < 64; y++) {
		for(uint32_t x = 0; x < 64; x++) {
			if(image[4 * (64 * y + x) + 3] < 0x80) {
				// Simple Alpha Testing
				continue;
			}
			color_t c = {
				image[4 * (64 * y + x) + 0],
				image[4 * (64 * y + x) + 1],
				image[4 * (64 * y + x) + 2]
			};
			SYSCALLS.video_setpixel(x, y, c);
		}
	}
	SYSCALLS.video_swap();
}

mainfunction_t load_elf(multiboot::Module const & module)
{
	elf::Header const &file = *module.start.data<elf::Header const>();
	if(file.magic != elf::MAGIC)
	{
		Console::main << "Invalid ELF magic.\n";
		return NULL;
	}
	Console::main << "ELF Version: " << file.version << "\n";
	
	
	elf::ProgramHeader const *header  = (elf::ProgramHeader const *)(module.start.numeric() + file.ph_offset);
	
	using namespace console_tools;
	for(uint32_t i = 0; i < file.ph_entry_count; i++, header++) {
	
		Console::main 
			<< "Program Header " << (uint32_t)i << ": " 
			<< hex(header->type) << ", "
			<< hex(header->virt_addr) << "@" << hex(header->offset)
			<< "\n";
		/* Nur Program Header vom Typ LOAD laden */
		if (header->type != 1) {
			if(header->type == 0x10) {
				blitIcon(((char*)&file) + header->offset);
			}
			continue;
		}
		
		if(header->virt_addr < USERSTART) {
			Console::main << "ELF Section below USERSTART, skipping...\n";
			continue;
		}
	
		void* dest = (void*)header->virt_addr;
		void* src = ((char*)&file) + header->offset;
		
		memset(dest, 0, header->mem_size);
		memcpy(dest, src, header->file_size);
	}
	
	return (mainfunction_t)file.entry;
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

extern "C" void get_memlimits(memlimits_t *limits)
{
	if(limits == nullptr) return;
	*limits = { USERSTART, RAMEND };
}

struct syscalls SYSCALLS = 
{
	&get_memlimits,
	&kbd_getchar,
	&kbd_is_pressed,
	&kbd_getkey,
	&video_clear,
	&video_buffer,
	&video_getmode,
	&video_setpixel,
	&video_swap,
	&timer_get,
	&timer_reset,
	
	&fs_open,
	&fs_type,
	&fs_info,
	&fs_close,
	
	&file_read,
	&file_size,
	
	&dir_length,
	&dir_get,
	
	&_puts,
};