#include <inttypes.h>

uint16_t *video = (uint16_t*)0xB8000;

void _start()
{
	for(int i = 0; i < 256; i++) {
		video[i] = 0xF000 | i;
	}
	while(1);
}