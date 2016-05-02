#include <inttypes.h>

#include "console.hpp"
#include "compat.h"

const char * numbers[] = {
  "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
};

extern "C" void init(void)
{
	Console::main
		<< "Hello World!\n"
		<< FColor(Color::Yellow) << "Hello color!" << FColor() << "\n"
		<< BColor(Color::Blue) << "Hello blue!" << BColor() << "\n"
		<< "Hello default color.\n";
}
