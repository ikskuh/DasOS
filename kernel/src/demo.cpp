#include "console.hpp"
#include "asm.hpp"
#include "idt.hpp"
#include "io.hpp"

#include "syscalls.h"

void dasos_demo()
{
	ASM::sti();

	while(true)
	{
		keyhit_t key;
		SYSCALLS.getkey(&key, true);
	
		if(key.flags & KHF_KeyPress) {
			Console::main << "Out of interrupt: " << (uint32_t)key.key.key;
			if(SYSCALLS.is_pressed(VK_Escape)) {
				Console::main << " with escape pressed";
			}
			Console::main << "\n";
		}
		if(key.flags & KHF_CharInput) {
			Console::main << "'" << (char)key.codepoint << "'\n";
		}
	}
	
	while(true)
	{
		Console::main << (char)SYSCALLS.getchar(); 
	}
}
