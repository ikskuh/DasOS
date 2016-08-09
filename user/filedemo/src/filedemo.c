#include <stdint.h>
#include "syscalls.h"

void main()
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