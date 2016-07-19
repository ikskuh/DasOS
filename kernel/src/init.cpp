#include <stdint.h>

uint16_t *video = (uint16_t*)0xB8000;

extern "C" void init()
{
	video[0] = 0xF000 | 'H';
	video[1] = 0xF000 | 'i';
	video[2] = 0xF000 | '!';

	while(1);
}