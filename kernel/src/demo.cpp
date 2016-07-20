#include "console.hpp"
#include "asm.hpp"
#include "idt.hpp"

void timer_intr(CpuState * & cpu)
{
	Console::main << "tick!\n";
}


void kbd_intr(CpuState * & cpu)
{
	Console::main << "tick!\n";
}

void dasos_demo()
{
	IDT::interrupt(0x20) = Interrupt(timer_intr);
	IDT::interrupt(0x21) = Interrupt(kbd_intr);


	ASM::sti();



	while(1);
}