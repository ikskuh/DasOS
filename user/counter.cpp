
#include <stdint.h>
#include "syscalls.h"

#define puts (SYSCALLS.puts)
#define getkey (SYSCALLS.getkey)

extern "C" void main()
{
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
