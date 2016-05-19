#include "vm.h"

#include <stdlib.h>
#include <stdio.h>

Instruction code[] = 
{
	// { VM_EXEC_X, VM_EXEC_X, VM_INPUT_ZERO, VM_INPUT_ZERO, VM_CMD_COPY, 0, VM_FLAG_NO, VM_OUTPUT_DISCARD, 0 },
	{ // push 0
		VM_EXEC_X, 
		VM_EXEC_X, 
		VM_INPUT_ARG, 
		VM_INPUT_ZERO, 
		VM_CMD_COPY, 
		0, 
		VM_FLAG_NO, 
		VM_OUTPUT_PUSH, 
		0
	},
	{ // dup
		VM_EXEC_X, 
		VM_EXEC_X, 
		VM_INPUT_PEEK, 
		VM_INPUT_ZERO, 
		VM_CMD_COPY, 
		0, 
		VM_FLAG_NO, 
		VM_OUTPUT_PUSH, 
		0
	},
	{ // cmp 10
		VM_EXEC_X, 
		VM_EXEC_X, 
		VM_INPUT_ARG, 
		VM_INPUT_POP, 
		VM_CMD_MATH, 
		VM_MATH_SUB, 
		VM_FLAG_YES, 
		VM_OUTPUT_DISCARD, 
		10
	},
	{ // jmp 7
		VM_EXEC_1,
		VM_EXEC_X, 
		VM_INPUT_ARG, 
		VM_INPUT_ZERO, 
		VM_CMD_COPY, 
		0, 
		VM_FLAG_NO, 
		VM_OUTPUT_JUMP, 
		7
	},
	{ // add 1
		VM_EXEC_X,
		VM_EXEC_X,
		VM_INPUT_ARG, 
		VM_INPUT_POP, 
		VM_CMD_MATH, 
		0,
		VM_FLAG_NO,
		VM_OUTPUT_PUSH, 
		1
	},
	{ // jmp 1
		VM_EXEC_X,
		VM_EXEC_X, 
		VM_INPUT_ARG, 
		VM_INPUT_ZERO, 
		VM_CMD_COPY, 
		0, 
		VM_FLAG_NO, 
		VM_OUTPUT_JUMP, 
		1
	},
};

void dump_proc(Process *process)
{
	if(process->flags & VM_FLAG_Z)
		printf("z");
	else
		printf(" ");
		
	if(process->flags & VM_FLAG_N)
		printf("n");
	else
		printf(" ");

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
	Module module = { code, 6 };

	Process process = { &module, 0, 0, 0, 0 };
	Process *p = &process;
	
	dump_proc(p);
	while(vm_step_process(p)) {
		dump_proc(p);
	}
	dump_proc(p);
	
	return 0;
}