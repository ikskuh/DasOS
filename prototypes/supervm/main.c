#include "vm.h"

#include <stdlib.h>
#include <stdio.h>

Instruction code[] = 
{
	{ VM_EXEC_X, VM_EXEC_X, VM_INPUT_ZERO, VM_INPUT_ZERO, VM_CMD_COPY, 0, VM_FLAG_NO, VM_OUTPUT_DISCARD, 0 },
};

void dump_proc(Process *process)
{
	printf("[ ");
	for(int i = 1; i <= process->stackPointer; i++) {
		printf("%d ", process->stack[i]);
	}
	printf("]\n");
}

void vm_assert(int assertion, const char *msg)
{
	if(assertion != 0)
		return;
	printf("Assertion failed: %s\n", msg);
	exit(1);
}

int main(int argc, const char **argv)
{
	Module module = { code, 1 };

	Process process = { &module, 0, 0, 0, 0 };
	Process *p = &process;
	
	int stillAlive = vm_step_process(p);
	
	printf("Process %s.\n", (stillAlive ? "still running" : "terminated"));
	
	return 0;
}