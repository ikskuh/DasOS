#pragma once

#include <stddef.h>
#include <inttypes.h>

#define ASM_READ_REG(reg, var) asm volatile("mov %%" #reg ", %0" : "=r" (var));
#define ASM_WRITE_REG(reg, var) asm volatile("mov %0, %%" #reg : : "r" (var));

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
	
	static inline void invlpg(void* m)
	{
			/* Clobber memory to avoid optimizer re-ordering access before invlpg, which may cause nasty bugs. */
			asm volatile ( "invlpg (%0)" : : "b"(m) : "memory" );
	}
	
	static inline void lidt(void *idt, size_t length)
	{
		struct {
			uint16_t limit;
			void* pointer;
		} __attribute__((packed)) idtp = {
			.limit = uint16_t(length * 8 - 1),
			.pointer = idt,
		};
		asm volatile("lidt %0" : : "m" (idtp));
	}
}