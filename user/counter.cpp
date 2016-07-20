
#include <stdint.h>
#include "console.hpp"

extern "C" void main()
{
	Console::main << FColor(Color::LightRed) << "Hello World!\n" << FColor(Color::White);
}