#include <inttypes.h>

#include "console.hpp"
#include "compat.h"

extern "C" void init(void)
{
	Console::main
		<< "Hello World!\n"
		<< FColor(Color::Yellow) << "Hello color!" << FColor() << "\n"
		<< BColor(Color::Blue) << "Hello blue!" << BColor() << "\n"
		<< "Hello default color.";
}
