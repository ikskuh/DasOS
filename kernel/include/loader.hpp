#pragma once


static const uint32_t USERSTART = 0x40000000;

extern "C" {
	typedef void (*mainfunction_t)(struct syscalls *syscalls);
	
	void loader_run(mainfunction_t main);
	
	void userspace_exit();
}

mainfunction_t load_elf(const char *fileName);