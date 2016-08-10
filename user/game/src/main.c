#include <stdint.h>
#include "syscalls.h"

typedef struct {
	uint32_t width, height, stride; // stride is in pixels, not bytes!
	const color_t * pixels;
} sprite_t;

void blit_sprite(const sprite_t * sprite, int x, int y)
{
	for(uint32_t dy = 0; dy < sprite->height; dy++) {
		for(uint32_t dx = 0; dx < sprite->width; dx++) {
			color_t color = sprite->pixels[sprite->stride * dy + dx];
			if(color.a >= 128) {
				video_setpixel(x + dx, y + dy, color);
			}
		}
	}
}

extern color_t iso_tree[];
extern color_t iso_gras[];

void main()
{
	color_t background = { 32, 64, 192};
	
	int x = 64;
	int y = 25 + 38;
	
	while(true)
	{
		if(is_pressed(vkW)) y--;
		if(is_pressed(vkS)) y++;
		
		if(is_pressed(vkA)) x--;
		if(is_pressed(vkD)) x++;
		
		if(is_pressed(vkEscape)) return;
	
		video_clear(background);
		
		// TODO: Implement game
		
		sprite_t gras = { 32, 32, 32, iso_gras };
		sprite_t tree = { 32, 32, 32, iso_tree };
		
		int lh = 19;
		for(int i = 0; i < 3; i++) {
			blit_sprite(&gras, 16, 16 + lh * i);
			blit_sprite(&gras, 48, 16 + lh * i);
			blit_sprite(&gras, 80, 16 + lh * i);
			blit_sprite(&gras, 112, 16 + lh * i);
			blit_sprite(&gras, 32, 25 + lh * i);
			blit_sprite(&gras, 64, 25 + lh * i);
			blit_sprite(&gras, 96, 25 + lh * i);
		}
		blit_sprite(&gras, 48, 72);
		blit_sprite(&gras, 80, 72);
		blit_sprite(&gras, 64, 80);
		
		blit_sprite(&tree, x, y);
		
		video_swap();
	}
	
	while(1);
}