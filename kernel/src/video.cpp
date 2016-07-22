#include "video.hpp"
#include "console.hpp"

using namespace console_tools;

vbe::ModeInfo videoMode;

void vid_init(vbe::ModeInfo const * mode)
{
	::videoMode = *mode;
	video_clear({0, 0, 255});
	
	Console::main
		<< "Video Info:\n"
		<< "  BPP:   " << (uint32_t)videoMode.bpp << "\n"
		<< "  Red:   " << hex((uint32_t)videoMode.redMask) << "@" << hex((uint32_t)videoMode.redPosition) << "\n"
		<< "  Green: " << hex((uint32_t)videoMode.greenMask) << "@" << hex((uint32_t)videoMode.greenPosition) << "\n"
		<< "  Blue:  " << hex((uint32_t)videoMode.blueMask) << "@" << hex((uint32_t)videoMode.bluePosition) << "\n";
}

static void set_pixel(uint32_t x, uint32_t y, color_t color)
{
	uint8_t * pixels = (uint8_t*)videoMode.framebuffer;
	if(x >= videoMode.res.x) return;
	if(y >= videoMode.res.y) return;
	
	uint32_t pixmask = (uint32_t)((1 << (int64_t)videoMode.bpp) - 1);
	
	int n = 1;
	if(videoMode.bpp >= 16) {
		n = 2;
	}
	if(videoMode.bpp >= 24) {
		n = 3;
	}
	if(videoMode.bpp >= 32) {
		n = 4;
	}
	
	uint32_t *pixel = (uint32_t*)&pixels[videoMode.pitch * y + n * x];
	
	*pixel = 
		(~pixmask & *pixel) |
		((((1<<videoMode.redMask) - 1) & color.r) << videoMode.redPosition) |
		((((1<<videoMode.greenMask) - 1) & color.g) << videoMode.greenPosition) |
		((((1<<videoMode.blueMask) - 1) & color.b) << videoMode.bluePosition);
}

void video_clear(color_t color)
{
	for(uint32_t y = 0; y < videoMode.res.y; y++) {
		for(uint32_t x = 0; x < videoMode.res.x; x++) {
			set_pixel(x, y, color);
		}
	}
}