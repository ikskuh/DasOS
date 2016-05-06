#include "bsod.hpp"
#include "console.hpp"
#include "exceptions.hpp"

static const char *toString(int interrupt)
{
	if(interrupt <= 0x1f) {
		switch(interrupt) {
#define EXCEPTION(num, shorthand, ident, desc, type) case num: return #desc;
#include "exceptions.lst"
#undef EXCEPTION
			default: return "Unknown Exception";
		}
	}
	if(interrupt >= 0x20 && interrupt <= 0x2F) {
		switch(interrupt - 0x20) {
#define IRQ(num, ident, desc) case num: return #desc;
#include "irqs.lst"
#undef IRQ
		}
	}
	return "Unknown Interrupt";
};

void BSOD::die(Error code, const char *msg)
{
  BSOD::die(code, msg, nullptr);
}

/**
 * Dies with a simple message and error code display.
 */
void BSOD::die(Error code, const char *msg, CpuState *cpu)
{
	using namespace console_tools;
	asm volatile ("cli");
	
	Console::main << FColor(Color::White) << BColor(Color::Red);
	Console::main.clear();
	Console::main
		<< "OH MY GOD. DasOS crashed! But i can tell you: \n"
		<< msg << "\n"
		<< "Also here is some CPU information:\n";
		
	Console::main 
			<< "eax    = " << hex(cpu->eax) << "\n"
			<< "ebx    = " << hex(cpu->ebx) << "\n"
			<< "ecx    = " << hex(cpu->ecx) << "\n"
			<< "edx    = " << hex(cpu->edx) << "\n"
			<< "esi    = " << hex(cpu->esi) << "\n"
			<< "edi    = " << hex(cpu->edi) << "\n"
			<< "ebp    = " << hex(cpu->ebp) << "\n"
			<< "intr   = " << cpu->interrupt << "(" << toString(cpu->interrupt) << ")" << "\n"
			<< "error  = " << cpu->error << "\n"
			<< "eip    = " << hex(cpu->eip) << "\n"
			<< "cs     = " << hex(cpu->cs) << "\n"
			<< "eflags = " << bin(cpu->eflags) << "\n"
			<< "esp    = " << hex(cpu->esp) << "\n"
			<< "ss     = " << hex(cpu->ss) << "\n";
	
	asm volatile ("hlt");
	while(true);
}