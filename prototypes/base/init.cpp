#include <inttypes.h>

#include "console.hpp"
#include "pmm.hpp"
#include "numeric.hpp"
#include "pointer.hpp"
#include "multiboot.hpp"
#include "compat.h"

using namespace multiboot;
using namespace console_tools;

extern "C" void init(Structure const & data)
{
	Console::main
		<< "Hello World!\n"
		<< FColor(Color::Yellow) << "Hello color!" << FColor() << "\n"
		<< BColor(Color::Blue) << "Hello blue!" << BColor() << "\n"
		<< "Hello default color.\n";
  
  Console::main
    << "bootloader name:  " << data.bootLoaderName << "\n"
    << "command line:     " << data.commandline << "\n"
    << "count of modules: " << data.modules.length << "\n"
    << "count of mmaps:   " << data.memoryMaps.length << "\n";
  for(auto &mmap : data.memoryMaps) {
    if(mmap.length == 0) {
      continue;
    }
    Console::main
      << "mmap: "
      << "start: " << hex(mmap.base) << ", length: " << hex(mmap.length)
      << ", " << mmap.entry_size
      << ", " << sizeof(mmap)
      << ", free:" << mmap.isFree()
      << "\n";
    if(mmap.base > 0xFFFFFFFF) {
        Console::main << "mmap out of 4 gigabyte range." << "\n";
        continue;
    }    
    if(mmap.isFree()) {      
      // Mark all free memory free...
      uintptr_t lower = (mmap.base + 0x0FFF) & 0xFFFFF000; // align at upper 4096
      uintptr_t upper = (mmap.base + mmap.length) & 0xFFFFF000; // align at lower 4096
      while (lower < upper) {
          PMM::markFree(physical_t(lower));
          lower += 0x1000;
      }
    }
  }
  
  for(int i = 0; i < 10; i++) {
    bool success;
    physical_t page = PMM::alloc(success);
    Console::main << "allocated page " << i << " [" << success << "]: " << page << "\n";
  }
}

static_assert(sizeof(void*) == 4, "Target platform is not 32 bit.");