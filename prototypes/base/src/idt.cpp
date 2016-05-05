#include "idt.hpp"
#include "io.hpp"
#include "console.hpp"

#define ISR(num) extern "C" void isr_##num();
#define ISR_ERR(num) ISR(num)
#include "../interrupt-list.inc"
#undef ISR
#undef ISR_ERR

static InterruptDescriptor invalid;
InterruptDescriptor IDT::descriptors[length];

InterruptDescriptor & IDT::descriptor(uint32_t idx)
{
	if(idx >= IDT::length) {
		return invalid;
	}
	return IDT::descriptors[idx];
}

void IDT::initialize()
{
#define ISR(num) IDT::descriptor(num) = InterruptDescriptor( \
		uint32_t(&isr_##num), \
		0x08, \
		InterruptFlags::Interrupt | InterruptFlags::Use32Bit | InterruptFlags::Ring0 | InterruptFlags::Present);
#define ISR_ERR(num) ISR(num)
#include "../interrupt-list.inc"
#undef ISR
#undef ISR_ERR

	struct {
		uint16_t limit;
		void* pointer;
	} __attribute__((packed)) idtp = {
		.limit = IDT::length * sizeof(InterruptDescriptor) - 1,
		.pointer = IDT::descriptors,
	};
	asm volatile("lidt %0" : : "m" (idtp));
	
	IDT::setupPIC();
}

void IDT::setupPIC()
{
	outb(0x20, 0x11);
	outb(0x21, 0x20);
	outb(0x21, 0x04);
	outb(0x21, 0x01);

	// Slave-PIC
	outb(0xa0, 0x11);
	outb(0xa1, 0x28);
	outb(0xa1, 0x02);
	outb(0xa1, 0x01);

	// Demask all interrupts
	outb(0x20, 0x0);
	outb(0xa0, 0x0);
}

void IDT::dispatch(CpuState *cpu)
{
	using namespace console_tools;

	Console::main << "Ermahgerd, Interrupts!\n";
	
	Console::main 
		<< "eax    = " << hex(cpu->eax) << "\n"
		<< "ebx    = " << hex(cpu->ebx) << "\n"
		<< "ecx    = " << hex(cpu->ecx) << "\n"
		<< "edx    = " << hex(cpu->edx) << "\n"
		<< "esi    = " << hex(cpu->esi) << "\n"
		<< "edi    = " << hex(cpu->edi) << "\n"
		<< "ebp    = " << hex(cpu->ebp) << "\n"
		<< "intr   = " << cpu->intr << "\n"
		<< "error  = " << cpu->error << "\n"
		<< "eip    = " << hex(cpu->eip) << "\n"
		<< "cs     = " << hex(cpu->cs) << "\n"
		<< "eflags = " << bin(cpu->eflags) << "\n"
		<< "esp    = " << hex(cpu->esp) << "\n"
		<< "ss     = " << hex(cpu->ss) << "\n";
	
	while(true);
}