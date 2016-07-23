#undef DASOS_KERNEL
#include "syscalls.h"

void dasos_demo()
{
	color_t bg = { 64, 64, 0 };
	color_t fg = { 192, 192, 192 };
	while(true)
	{
		video_clear(bg);
	}

	while(true)
	{
		keyhit_t key;
		getkey(&key, true);
	
		if(key.flags & KHF_KeyPress) {
			// Console::main << "Out of interrupt: " << (uint32_t)key.key.key;
			if(is_pressed(VK_Escape)) {
				// Console::main << " with escape pressed";
			}
			// Console::main << "\n";
		}
		if(key.flags & KHF_CharInput) {
			// Console::main << "'" << (char)key.codepoint << "'\n";
		}
	}
	
	while(true)
	{
		// Console::main << (char)getchar(); 
	}
}
