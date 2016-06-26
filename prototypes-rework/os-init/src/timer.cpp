#include <driver/timer.hpp>
#include <kernel/idt.hpp>

namespace driver
{
	static volatile uint32_t counter = 0;
	
	Timer::Timer()
	{
	
	}

	void Timer::install()
	{
		IDT::interrupt(0x20) = Interrupt([](auto *&) { counter++; });
	}
	
	void Timer::reset()
	{
		counter = 0;
	}
		
	uint32_t count()
	{
		return counter;
	}
	
}