#include "syscalls.h"

struct syscalls SYSCALLS;

void main();

memlimits_t __memoryLimits;

void __usrinit(struct syscalls * syscalls)
{
	SYSCALLS = *syscalls;
	SYSCALLS.get_memlimits(&__memoryLimits);
	main();
}