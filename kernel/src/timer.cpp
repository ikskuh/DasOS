#include "timer.hpp"
#include "idt.hpp"

static volatile uint32_t counter = 0;

static void timer_intr(CpuState * &cpu)
{
	counter++;
}

void timer_init()
{
	IDT::interrupt(0x20) = Interrupt(timer_intr);
	::counter = 0;
}

extern "C" uint32_t timer_get()
{
	return ::counter;
}

extern "C" void timer_reset()
{
	::counter = 0;
}