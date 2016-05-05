#include "idt.hpp"

#include "console.hpp"

static InterruptDescriptor invalid;
InterruptDescriptor IDT::descriptors[length];

InterruptDescriptor & IDT::descriptor(uint32_t idx)
{
	if(idx >= IDT::length) {
		return invalid;
	}
	return IDT::descriptors[idx];
}

static void trap()
{
	Console::main << "trapped!";
	while(true);
}

void IDT::initialize()
{
	IDT::descriptor(0) = InterruptDescriptor(
		uint32_t(&trap), // offset
		0x08, // segment
		InterruptFlags::Interrupt | InterruptFlags::Use32Bit | InterruptFlags::Ring0 | InterruptFlags::Present);
		
	IDT::descriptor(1) = InterruptDescriptor(
		uint32_t(&trap), // offset
		0x08, // segment
		InterruptFlags::Interrupt | InterruptFlags::Use32Bit | InterruptFlags::Ring0 | InterruptFlags::Present);
	
	
	struct {
		uint16_t limit;
		void* pointer;
	} __attribute__((packed)) idtp = {
		.limit = IDT::length * sizeof(InterruptDescriptor) - 1,
		.pointer = IDT::descriptors,
	};
	asm volatile("lidt %0" : : "m" (idtp));
}