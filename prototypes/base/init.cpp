#include <inttypes.h>

#include "console.hpp"
#include "pmm.hpp"
#include "numeric.hpp"
#include "pointer.hpp"
#include "multiboot.hpp"
#include "compat.h"

using namespace multiboot;

extern "C" void init(Structure const * data)
{
	Console::main
		<< "Hello World!\n"
		<< FColor(Color::Yellow) << "Hello color!" << FColor() << "\n"
		<< BColor(Color::Blue) << "Hello blue!" << BColor() << "\n"
		<< "Hello default color.\n";
  
  Console::main
    << "multiboot structure: 0x" << data << "\n"
    << "bootloader name:     " << data->bootLoaderName << "\n"
    << "command line:        " << data->commandline << "\n"
    << "count of modules:    " << data->modules.length << "\n";
  
  for(const Module &module : data->modules) {
    Console::main
      << "Module " << module.name << "\n";
  }
  
  /*
  PMM::markOccupied(physical_t(0x1500));
  for(int i = 0; i < 10; i++) {
    bool success;
    physical_t page(PMM::alloc(success));
    Console::main << "allocated page " << i << " [" << success << "]: 0x" << page.data() << "\n";
  }
  */
}

static_assert(sizeof(void*) == 4, "Target platform is not 32 bit.");