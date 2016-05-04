#pragma once

static inline void outb(uint16_t port, uint8_t data)
{
	__asm__ volatile ("outb %0, %1" : : "a" (data), "Nd" (port));
}

static inline uint8_t inb(uint16_t port)
{
	uint8_t data;
	__asm__ volatile ("inb %1, %0" : "=a" (data) : "d" (port));
	return data;
}