#include <inttypes.h>

#include "console.hpp"
#include "pmm.hpp"
#include "numeric.hpp"
#include "pointer.hpp"
#include "compat.h"

extern "C" void init(void)
{
	Console::main
		<< "Hello World!\n"
		<< FColor(Color::Yellow) << "Hello color!" << FColor() << "\n"
		<< BColor(Color::Blue) << "Hello blue!" << BColor() << "\n"
		<< "Hello default color.\n";
  
  PMM::markOccupied((void*)0x1500);
  
  for(int i = 0; i < 10; i++) {
    bool success;
    physical_t page(PMM::alloc(success));
    Console::main << "allocated page " << i << " [" << success << "]: 0x" << page.data() << "\n";
  }
}

static_assert(sizeof(void*) == 4, "Target platform is not 32 bit.");