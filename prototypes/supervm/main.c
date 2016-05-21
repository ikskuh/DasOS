#include "vm.h"

#include <stdlib.h>
#include <stdio.h>

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

void vm_assert(int assertion, const char *msg)
{
	if(assertion != 0)
		return;
	printf("Assertion failed: %s\n", msg);
	exit(1);
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
	
	Process process = { &module, 0, 0, 0, 0 };
	Process *p = &process;
	
	dump_proc(p);
	while(vm_step_process(p)) {
		dump_proc(p);
	}
	dump_proc(p);
	
	return 0;
}