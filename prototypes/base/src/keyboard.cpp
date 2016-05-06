#include "driver/keyboard.hpp"
#include "idt.hpp"
#include "console.hpp"

namespace driver
{
	Keyboard::Keyboard()
	{

	}

	void Keyboard::install()
	{
		IDT::interrupt(0x21) = Interrupt(Keyboard::dispatchIRQ);
	}
	
	void Keyboard::dispatchIRQ(CpuState *cpu)
	{
		Console::main << "keyboard! ";
	}
}