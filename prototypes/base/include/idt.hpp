#pragma once

#include <inttypes.h>
#include "enums.hpp"
#include "cpustate.hpp"

enum class InterruptFlags : uint8_t
{
	None      = 0x00,
	Interrupt = 0x06,
	TrapGate  = 0x07,
	TaskGate  = 0x05,
	Use32Bit  = 0x08,
	Ring0     = 0x00,
	Ring1     = 0x20,
	Ring2     = 0x40,
	Ring3     = 0x60,
	Present   = 0x80,
};

ENUM_CLASS_OPERATORS(InterruptFlags)

struct InterruptDescriptor
{
	uint16_t offset0;
	uint16_t selector;
	uint8_t zero;
	InterruptFlags flags;
	uint16_t offset1;
	
	InterruptDescriptor() : 
		offset0(0), selector(0), zero(0), flags(InterruptFlags::None), offset1(0)
	{
	
	}
	
	InterruptDescriptor(uint32_t offset, uint32_t selector, InterruptFlags flags) : 
		offset0(0), selector(selector), zero(0), flags(flags), offset1(0)
	{
		this->setOffset(offset);
	}
	
	uint32_t offset() const
	{
		return this->offset0 | (this->offset1 << 16);
	}
	
	void setOffset(uint32_t offset)
	{
		this->offset0 = (offset & 0x0000FFFF) >> 0;
		this->offset1 = (offset & 0xFFFF0000) >> 16;
	}
	
} __attribute__((packed));

static_assert(sizeof(InterruptDescriptor) == 8, "InterruptDescriptor must be 8 byte large.");


class IDT
{
public:
	static const uint32_t length = 256;
private:
	static InterruptDescriptor descriptors[length];
	IDT() = delete;
	
	static void dispatch(CpuState *cpu);
public:

	static InterruptDescriptor & descriptor(uint32_t idx);

	static void initialize();
	
};