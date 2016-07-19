#include <stdint.h>
#include <new>
#include "multiboot.hpp"
#include "pmm.hpp"
#include "vmm.hpp"
#include "console.hpp"

VMMContext * kernelContext;

uint16_t *video = (uint16_t*)0xB8000;

uint32_t LOWMEM;
uint32_t HIGHMEM;

void initialize_pmm(multiboot::Structure const & mb);

void initialize_vmm();

extern "C" void init(multiboot::Structure const & mb)
{
	initialize_pmm(mb);
	
	initialize_vmm();
	
	Console::main
		<< "LowMem:  " << LOWMEM << "\n"
		<< "HighMem: " << HIGHMEM << "\n";
	
	while(1);
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
	for(uint32_t i = 0; i < userspace_size; i++) {
		
		kernelContext->map(
			virtual_t(HIGHMEM),
			PMM::alloc(),
			VMMFlags::Writable);
		
		HIGHMEM += 0x1000;
	}
		
	Console::main << "Active Context...\n";
	VMM::activate(*kernelContext);
	
	Console::main << "Active Paging...\n";
	VMM::enable();
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