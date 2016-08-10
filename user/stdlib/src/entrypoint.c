#include "syscalls.h"

struct syscalls SYSCALLS;

void main();

memlimits_t __memoryLimits;

extern const void heap_start;
uint32_t memCursor;

void *sbrk(intptr_t increment)
{
	uint32_t val = memCursor;
	memCursor += increment;
	return (void*)val;
}

void _daos_entry(struct syscalls * syscalls)
{
	SYSCALLS = *syscalls;
	SYSCALLS.get_memlimits(&__memoryLimits);
	
	memCursor = (uint32_t)&heap_start + 0x1000;
	
	main();
}