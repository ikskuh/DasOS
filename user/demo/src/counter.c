#include <stdint.h>
#include "syscalls.h"

void main()
{
	color_t background = {255, 0, 0};
	video_clear(background);
	// video_swap(); // Required for back buffering
	
	puts("Hello World!\n");
	while(true)
	{
		keyhit_t hit;
		getkey(&hit, true);
		if(hit.key.key == VK_Escape) {
			break;
		}
		else if(hit.key.key == VK_Space) {
			if(hit.flags & KHF_KeyPress) {
				puts("Space was pressed!\n");
			} else {
				puts("Space was released!\n");
			}
		}
	}
}