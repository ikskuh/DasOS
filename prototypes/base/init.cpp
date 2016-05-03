#include <inttypes.h>

#include "console.hpp"
#include "pmm.hpp"
#include "numeric.hpp"
#include "compat.h"

extern "C" void init(void)
{
	Console::main
		<< "Hello World!\n"
		<< FColor(Color::Yellow) << "Hello color!" << FColor() << "\n"
		<< BColor(Color::Blue) << "Hello blue!" << BColor() << "\n"
		<< "Hello default color.\n";
  for(int i = 0; i < 4097; i++) {
    bool success;
    void *page = PMM::alloc(success);
    Console::main << "allocated page " << i << " [" << success << "]: 0x" << page << "\n";
  }
}

static_assert(sizeof(void*) == 4, "Target platform is not 32 bit.");