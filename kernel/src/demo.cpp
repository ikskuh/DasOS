#undef DASOS_KERNEL
#include "syscalls.h"

#include "console.hpp"

static void video_sync()
{
	uint32_t end = timer_get() + 0;
	while(end >= timer_get());
}

void dasos_demo()
{
	color_t bg = { 64, 64, 0 };
	color_t fg = { 192, 192, 192 };
		
	int x = 50;
	int y = 50;
	
	while(true)
	{
		if(is_pressed(VK_A)) x -= 1;
		if(is_pressed(VK_D)) x += 1;
		if(is_pressed(VK_W)) y -= 1;
		if(is_pressed(VK_S)) y += 1;
	
		video_clear(bg);
		
		for(int i = 0; i < 100; i++) {
			
			video_setpixel(x + i, y, fg);
			video_setpixel(x, y + i, fg);
			video_setpixel(x + 100, y + i, fg);
			video_setpixel(x + i, y + 100, fg);
		}
		
		// video_sync();
		video_swap();
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
