#include "syscalls.h"

struct syscalls SYSCALLS;

void main();

void __usrinit(struct syscalls * syscalls)
{
	SYSCALLS = *syscalls;
	main();
}