#include "vm.h"

int vm_step_process(Process *process)
{
	vm_assert(process != NULL, "process must not be NULL.");
	Instruction instr = process->module->code[process->codePointer];

	return 0;
}

void vm_push(Process *process, uint32_t value)
{
	vm_assert(process != NULL, "process must not be NULL.");
	vm_assert(process->stackPointer < VM_STACKSIZE, "Stack overflow");
	process->stack[++process->stackPointer] = value;
}

uint32_t vm_pop(Process *process)
{
	vm_assert(process != NULL, "process must not be NULL.");
	uint32_t psp = process->stackPointer;
	uint32_t val = process->stack[process->stackPointer--];
	
	// Underflow check works because unsigned overflow is defined ;)
	vm_assert(psp >= process->stackPointer, "Stack underflow");
	
	return val;
}

uint32_t vm_peek(Process *process)
{
	vm_assert(process != NULL, "process must not be NULL.");
	return process->stack[process->stackPointer];
}
