#include "vm.h"

typedef struct 
{
	uint32_t input0;
	uint32_t input1;
	uint32_t argument;
	
	uint32_t output;
} CommandInfo;

static void cmd_copy(CommandInfo *info)
{
	info->output = info->input0;
}

int vm_step_process(Process *process)
{
	vm_assert(process != NULL, "process must not be NULL.");
	Instruction instr = process->module->code[process->codePointer++];

	int exec = 1;
	switch(instr.execZ)
	{
		case VM_EXEC_X:
			/* Don't modify execution. */
			break;
		case VM_EXEC_0:
			if((process->flags & VM_FLAG_Z) != 0)
				exec = 0;
			break;
		case VM_EXEC_1:
			if((process->flags & VM_FLAG_Z) == 0)
				exec = 0;
			break;
		default:
			vm_assert(0, "Invalid instruction: execZ undefined.");
			break;
	}
	
	switch(instr.execN)
	{
		case VM_EXEC_X:
			/* Don't modify execution. */
			break;
		case VM_EXEC_0:
			if((process->flags & VM_FLAG_N) != 0)
				exec = 0;
			break;
		case VM_EXEC_1:
			if((process->flags & VM_FLAG_N) == 0)
				exec = 0;
			break;
		default:
			vm_assert(0, "Invalid instruction: execN undefined.");
			break;
	}
	
	// Only do further instruction execution when
	// the execution condition is met.
	if(exec)
	{
		CommandInfo info;
		switch(instr.input0)
		{
			case VM_INPUT_ZERO: info.input0 = 0; break;
			case VM_INPUT_POP:  info.input0 = vm_pop(process); break;
			case VM_INPUT_PEEK: info.input0 = vm_peek(process); break;
			case VM_INPUT_ARG:  info.input0 = instr.argument; break;
			default: vm_assert(0, "Invalid instruction: input0 undefined.");
		}
		
		switch(instr.input1)
		{
			case VM_INPUT_ZERO: info.input1 = 0; break;
			case VM_INPUT_POP:  info.input1 = vm_pop(process); break;
			default: vm_assert(0, "Invalid instruction: input1 undefined.");
		}
		
		info.argument = instr.argument;
		
		switch(instr.command)
		{
			case VM_CMD_COPY: cmd_copy(&info); break;
			default: vm_assert(0, "Invalid instruction: command undefined.");
		}
		
		switch(instr.flags)
		{
			case VM_FLAG_YES:
				process->flags = 0;
				if(info.output == 0)
					process->flags |= VM_FLAG_Z;
				else if((info.output & (1<<31)) != 0)
					process->flags |= VM_FLAG_N;
				break;
			case VM_FLAG_NO: break;
			default:
				vm_assert(0, "Invalid instruction: invalid flags.");
		}
		
		switch(instr.output)
		{
			case VM_OUTPUT_DISCARD: break;
			case VM_OUTPUT_PUSH: vm_push(process, info.output); break;
			case VM_OUTPUT_JUMP: process->codePointer = info.output; break;
			default:
				vm_assert(0, "Invalid instruction: invalid output.");
		}
	}
	
	return process->codePointer < process->module->length;
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
