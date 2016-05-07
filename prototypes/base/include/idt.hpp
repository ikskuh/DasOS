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
	
	InterruptDescriptor();
	
	InterruptDescriptor(uint32_t offset, uint32_t selector, InterruptFlags flags);
	
	uint32_t offset() const;
	
	void setOffset(uint32_t offset);
} __attribute__((packed));

static_assert(sizeof(InterruptDescriptor) == 8, "InterruptDescriptor must be 8 byte large.");

/**
 * An interrupt that specifies
 * a handler and if the interrupt is
 * enabled. Any non-enabled interrupt
 * causes a BSOD.
 */
class Interrupt
{
	friend class IDT;
	using Handler = void (*)(CpuState * & cpu);
private:
	bool isEnabled;
	Handler handler;
	
public:
	Interrupt();
	explicit Interrupt(Handler handler);
};


class IDT
{
public:
	static const uint32_t length = 256;
	
	static Interrupt interrupts[length];
	
private:
	static InterruptDescriptor descriptors[length];
	IDT() = delete;
	
	static CpuState * dispatch(CpuState *cpu);
	static void setupPIC();
public:

	/**
	 * Accessor to an interrupt handler.
	 */
	static Interrupt & interrupt(uint32_t index);

	/** 
	 * Gets an interrupt descriptor
	 */
	static InterruptDescriptor & descriptor(uint32_t idx);

	/**
	 * Initializes the interrupt table and sets up the PIC.
	 */
	static void initialize();
	
};