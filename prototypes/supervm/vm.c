#include "vm.h"

#include <stdio.h>

static void cmd_copy(CommandInfo *info)
{
	info->output = info->input0;
}

static void cmd_load(Process *p, CommandInfo *info)
{
	info->output = vm_read_byte(p, info->input0);
}

static void cmd_store(Process *p, CommandInfo *info)
{
	vm_write_byte(p, info->input0, info->input1);
	info->output = info->input1;
}

static void cmd_spget(Process *p, CommandInfo *info)
{
	info->output = p->stackPointer;
}

static void cmd_spset(Process *p, CommandInfo *info)
{
	info->output = p->stackPointer = info->input0;
}

static void cmd_bpget(Process *p, CommandInfo *info)
{
	info->output = p->basePointer;
}

static void cmd_bpset(Process *p, CommandInfo *info)
{
	info->output = p->basePointer = info->input0;
}

static void cmd_cpget(Process *p, CommandInfo *info)
{
	info->output = p->codePointer + info->additional;
}

static inline int16_t makeSigned(uint16_t val)
{
	return *((int16_t*)&val);
}

static void cmd_get(Process *p, CommandInfo *info)
{
	info->output = p->stack[p->basePointer + makeSigned(info->input0)];
}

static void cmd_set(Process *p, CommandInfo *info)
{
	info->output = p->stack[p->basePointer + makeSigned(info->input0)] = info->input1;
}

static void cmd_math(CommandInfo *info)
{
	switch(info->additional)
	{
		// IMPORTANT:
		// input1 - input0 because then input0 can be a fixed value 
#define S(name, op) case name: info->output = info->input1 op info->input0; break;
		S(VM_MATH_ADD, +)
		S(VM_MATH_SUB, -)
		S(VM_MATH_MUL, *)
		S(VM_MATH_DIV, /)
		S(VM_MATH_MOD, %)
		S(VM_MATH_AND, &)
		S(VM_MATH_OR, |)
		S(VM_MATH_XOR, ^)
#undef S
		case VM_MATH_NOT: info->output = ~info->input0; break;
		default: vm_assert(0, "Invalid instruction: MATH command not defined."); break;
	}
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
		CommandInfo info = { 0 };
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
		info.additional = instr.cmdinfo;
		
		switch(instr.command)
		{
			case VM_CMD_COPY: cmd_copy(&info); break;
			case VM_CMD_STORE: cmd_store(process, &info); break;
			case VM_CMD_LOAD: cmd_load(process, &info); break;
			case VM_CMD_MATH: cmd_math(&info); break;
			case VM_CMD_SYSCALL: vm_syscall(process, &info); break;
			case VM_CMD_HWIO: vm_hwio(process, &info); break;
			case VM_CMD_SPGET: cmd_spget(process, &info); break;
			case VM_CMD_SPSET: cmd_spset(process, &info); break;
			case VM_CMD_BPGET: cmd_bpget(process, &info); break;
			case VM_CMD_BPSET: cmd_bpset(process, &info); break;
			case VM_CMD_CPGET: cmd_cpget(process, &info); break;
			case VM_CMD_GET: cmd_get(process, &info); break;
			case VM_CMD_SET: cmd_set(process, &info); break;
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





uint8_t vm_read_byte(Process *process, uint32_t address)
{
	vm_assert(process != NULL, "process must not be NULL.");
	
	uint32_t page  = address / process->mmap.pageSize;
	uint32_t index = address % process->mmap.pageSize;
	vm_assert(page < process->mmap.length, "Out of memory.");
	
	return process->mmap.pages[page][index];
}

void vm_write_byte(Process *process, uint32_t address, uint8_t value)
{
	vm_assert(process != NULL, "process must not be NULL.");
	
	uint32_t page  = address / process->mmap.pageSize;
	uint32_t index = address % process->mmap.pageSize;
	vm_assert(page < process->mmap.length, "Out of memory.");
	
	process->mmap.pages[page][index] = value;
}