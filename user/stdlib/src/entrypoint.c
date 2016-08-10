#include "syscalls.h"
#include "stdlib.h"

struct syscalls SYSCALLS;

void main();

memlimits_t __memoryLimits;

extern const void heap_start;
uint32_t memCursor;

void *sbrk(intptr_t increment)
{
	static char buffer[256];
	
	strcpy(buffer, "sbrk(0x");
	strcat(buffer, itoa(memCursor, NULL, 16));
	strcat(buffer, ", 0x");
	strcat(buffer, itoa(increment, NULL, 16));
	strcat(buffer, ")\n");
	puts(buffer);
	
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