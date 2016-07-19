
#include <stdint.h>
#include "console.hpp"

uint16_t *video = (uint16_t*)0xB8000;

extern "C" void main()
{
	Console::main << FColor(Color::LightRed) << "Hello World!\n" << FColor(Color::White);
}