#include "bsod.hpp"
#include "console.hpp"
#include "exceptions.hpp"

static const char *toString(int interrupt)
{
	if(interrupt <= 0x1f) {
		switch(interrupt) {
#define EXCEPTION(num, shorthand, ident, desc, type) case num: return #desc;
#include <lists/exceptions.lst>
#undef EXCEPTION
			default: return "Unknown Exception";
		}
	}
	if(interrupt >= 0x20 && interrupt <= 0x2F) {
		switch(interrupt - 0x20) {
#define IRQ(num, ident, desc) case num: return #desc;
#include <lists/irqs.lst>
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
	FColor Y(Color::Yellow);
	FColor W(Color::White);
	
	asm volatile ("cli");
	
	uint32_t cr0 = 0xBADEAFFE, cr1 = 0xBADEAFFE, cr2 = 0xBADEAFFE, cr3 = 0xBADEAFFE;
	
	asm volatile("mov %%cr0, %0" : "=r" (cr0));
	// asm volatile("mov %%cr1, %0" : "=r" (cr1));
	asm volatile("mov %%cr2, %0" : "=r" (cr2));
	asm volatile("mov %%cr3, %0" : "=r" (cr3));
	
	Console::main << FColor(Color::White) << BColor(Color::Red);
	Console::main.clear();
	
	Console::main
		<< "                                ___  __  __  ___ " << "\n"
		<< "                               / _ \\|  \\/  |/ __|" << "\n"
		<< "                              | (_) | |\\/| | (_ |" << "\n"
		<< "                               \\___/|_|  |_|\\___|" << "\n"
		<< "\n";
	
	Console::main
		<< "DasOS crashed!\n"
		<< "\n"
		<< "CODE: " << Y;
	switch(code) {
	#define ERROR(num, ident, desc) case Error::ident: Console::main << #ident << " / " << #desc << "\n"; break;
#include <lists/errors.lst>
#undef ERROR
	}
	Console::main
		<< W << "MSG:  " << Y
		<< msg << W << "\n"
		<< "\n"
		<< "CPU State:\n";
		
		
	Console::main 
			<< W << "eax = " << Y << pad(hex(cpu->eax), 10)  << W << " esi = " << Y << pad(hex(cpu->esi), 10) << W << " edx = " << Y << pad(hex(cpu->edx), 10) << W << " esp = " << Y << pad(hex(cpu->esp), 10) << "\n"
			<< W << "ebx = " << Y << pad(hex(cpu->ebx), 10)  << W << " edi = " << Y << pad(hex(cpu->edi), 10) << W << " eip = " << Y << pad(hex(cpu->eip), 10) << W << " flg = " << Y << bin(cpu->eflags) << "\n"
			<< W << "ecx = " << Y << pad(hex(cpu->ecx), 10)  << W << " ebp = " << Y << pad(hex(cpu->ebp), 10) << W << " cs  = " << Y << pad(hex(cpu->cs), 10)  << W << " ss  = " << Y << pad(hex(cpu->ss), 10)  << "\n"
			<< W << "cr0 = " << Y << pad(hex(cr0), 10)       << W << " cr1 = " << Y << pad(hex(cr1), 10)      << W << " cr2 = " << Y << pad(hex(cr2), 10)      << W << " cr3 = " << Y << pad(hex(cr3), 10)      << W << "\n"
			<< W << "int = " << Y << pad(cpu->interrupt, 10) << W << " err = " << Y << pad(cpu->error, 10)    << W << " " << toString(cpu->interrupt);

	asm volatile ("hlt");
	while(true);
}