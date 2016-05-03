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
  for(int i = 0; i < 25; i++) {
    Console::main << (-i) << "\n";
  }
  Console::main << "addrof(init) = 0x" << reinterpret_cast<void*>(0xFF00FF) << "\n";
}

static_assert(sizeof(void*) == 4, "Target platform is not 32 bit.");