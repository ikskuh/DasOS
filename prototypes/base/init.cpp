#include <inttypes.h>

#include "console.hpp"
#include "compat.h"

extern "C" void init(void)
{
  const char * hw = "Hello World!\nHello new line!";
	Console::main << hw;
}
