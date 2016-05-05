#include "idt.hpp"
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
}

void IDT::dispatch(CpuState *cpu)
{
	Console::main << "Ermahgerd, Interrupts!";
	while(true);
}