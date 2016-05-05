#include "idt.hpp"
#include "io.hpp"
#include "bsod.hpp"
#include "pic.hpp"

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
	masterPIC.initialize(0x20, 0x04, 0x01);
	slavePIC.initialize(0x28, 0x02, 0x01);

	masterPIC.maskInterrupts(0x00);
	slavePIC.maskInterrupts(0x00);
}

void IDT::dispatch(CpuState *cpu)
{
	if(cpu->interrupt <= 0x1F) {
		// Exception Handling
		BSOD::die(Error::UnhandledException, "Ermahgerd, Exceptions!", cpu);
	} else if (cpu->interrupt >= 0x20 && cpu->interrupt <= 0x2F) {
	
		// IRQ
		// Console::main << "[IRQ " << (cpu->interrupt - 0x20) << "]";
	
		if(cpu->interrupt >= 0x28) {
			slavePIC.sendEndOfInterrupt();
		}
		masterPIC.sendEndOfInterrupt();
	} else {
		BSOD::die(Error::UnhandledInterrupt, "Ermahgerd, Interrupts!", cpu);
	}
}