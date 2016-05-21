#include "vm.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void vm_assert(int assertion, const char *msg)
{
	if(assertion != 0)
		return;
	printf("Assertion failed: %s\n", msg);
	exit(1);
}

void vm_syscall(Process *p, CommandInfo *info)
{
	switch(info->additional)
	{
		case 0: // EXIT
			p->tag = NULL;
			break;
		case 1:
			printf("%c", info->input0);
			break;
		case 2:
			printf("%d", info->input0);
			break;
		default:
			printf("Some syscall: (%d, %d, %d, %d)\n",
				info->input0,
				info->input1,
				info->argument,
				info->additional);
			break;
		}
}

void vm_hwio(Process *p, CommandInfo *info)
{
	printf("Some hardware IO: (%d, %d, %d, %d)\n",
		info->input0,
		info->input1,
		info->argument,
		info->additional);
}



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
	
	printf(
		"%2d %2d ",
		process->codePointer,
		process->basePointer);

	printf("[ ");
	for(int i = 1; i <= process->stackPointer; i++) {
		printf("%d ", process->stack[i]);
	}
	printf("]\n");
}

void dump_memory(Process *process)
{
	printf("Memory Dump:\n");
	
	for(int i = 0; i < process->mmap.length; i++) {
		printf("Page %d\n", i);
		
		// Just assume that pageSize is a multiple of 16
		for(int j = 0; j < process->mmap.pageSize; j++) {
			if(j % 16 == 0)
				printf("%3x ", j);
			
			printf("%2.2X ", process->mmap.pages[i][j]);
			
			if(j % 16 == 15) {
				printf(" '");
				for(int x = 16*(j/16); x < 16*(j/16+1); x++) {
					char c = process->mmap.pages[i][x];
					if(c < 0x20) c = '.';
					if(c > 0x7E) c = '.';
					printf("%c", c);
				}
				printf("'\n");
			}
		}
	}
}

int main(int argc, const char **argv)
{
	if(argc < 2) {
		printf("Usage: vm [binary]\n");
		printf("  [binary]: The file to be executed.\n");
		return 1;
	}
	Module module = { 0 };
	{ // Read module
		FILE *f = fopen(argv[1], "rb");
		fseek(f, 0, SEEK_END);
		size_t len = ftell(f);
		fseek(f, 0, SEEK_SET);
		module.code = malloc(len);
		module.length = len / sizeof(Instruction);
		
		char *ptr = (char*)module.code;
		for(int p = 0; p < len; p += fread(&ptr[p], 1, sizeof(Instruction), f));
		
		fclose(f);
	}
	printf("Loaded module %s with %d instructions.\n", argv[1], module.length);
	for(int i = 0; i < module.length; i++)
	{
		Instruction c = module.code[i];
		printf(
			"%3d: %10d %1d %1d %5d %3d %1d %1d %1d %1d\n", 
			i,
			c.argument,
			c.output, 
			c.flags, 
			c.cmdinfo,
			c.command,
			c.input1,
			c.input0,
			c.execN,
			c.execZ);
	}
	
	uint8_t page0[64];
	uint8_t page1[64];
	
	strcpy((char*)page0, "Hallo Welt!");
	
	uint8_t *mmapDirectory[2] = {
		page0,
		page1,
	};
	
	VirtualMemoryMap mmap = {
		.pageSize = 64,
		.length = 2,
		.pages = mmapDirectory,
	};
	
	Process process = { 
		&module, 
		.codePointer = 0,
		.stackPointer = 0, 
		.basePointer = 0, 
		.flags = 0, 
		.mmap = mmap,
	};
	
	Process *p = &process;
	p->tag = p;
	
	dump_memory(p);
	dump_proc(p);
	
	while(vm_step_process(p) && p->tag) {
		dump_proc(p);
	}
	dump_proc(p);
	dump_memory(p);
	
	return 0;
}