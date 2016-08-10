#include <stdint.h>
#include <stdbool.h>
#include "syscalls.h"

void main()
{
	while(true)
	{
		keyhit_t hit;
		getkey(&hit, true);
		
		
		if(hit.flags & khfKeyPress)
		{
			switch(hit.key.key)
			{
				case vkNumpad1: exec("C:/filedemo");
				case vkNumpad2: exec("C:/fontdemo");
				case vkNumpad3: exec("C:/game");
				default: break;
			}
		}
	}
}

void ls()
{
	puts("Listing root directory:\n");
	
	int fd = fs_open("C:/");
	if(fd == 0) {
		puts("Failed to list root directory.\n");
		return;
	}
	if(fs_type(fd) != ftDirectory) {
		puts("C:/ is not a directory?!\n");
		return;
	}
	
	int len = dir_length(fd);
	for(int i = 0; i < len; i++)
	{
		struct fsnode node;
		if(dir_get(fd, i, &node))
		{
			puts(node.name);
			puts("\n");
		}
	}
	
	puts("done.\n");
}