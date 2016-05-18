#pragma once

namespace ASM
{
	static inline void sti()
	{
		__asm__ volatile ("sti");
	}
	
	static inline void cli()
	{
		__asm__ volatile ("cli");
	}
	
	static inline void outb(uint16_t port, uint8_t data)
	{
		asm volatile ("outb %0, %1" : : "a" (data), "Nd" (port));
	}

	static inline uint8_t inb(uint16_t port)
	{
		uint8_t data;
		asm volatile ("inb %1, %0" : "=a" (data) : "d" (port));
		return data;
	}
}