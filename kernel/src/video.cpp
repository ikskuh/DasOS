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

static inline void set_pixel(uint32_t x, uint32_t y, color_t color)
{
	uint8_t * pixels = (uint8_t*)videoMode.framebuffer;
	if(x >= videoMode.res.x) return;
	if(y >= videoMode.res.y) return;
	
	uint32_t pixmask = (uint32_t)((1 << (int64_t)videoMode.bpp) - 1);
	
	int n = 1;
	switch(videoMode.bpp) {
		case 8:
			n = 1;
			break;
		case 15:
		case 16:
			n = 2;
			break;
		case 24:
			n = 3;
			break;
		case 32:
			n = 4;
			break;
	}
	uint32_t *pixel = (uint32_t*)&pixels[videoMode.pitch * y + n * x];
	
	*pixel = 
		(~pixmask & *pixel) |
		((((1<<videoMode.redMask) - 1) & color.r) << videoMode.redPosition) |
		((((1<<videoMode.greenMask) - 1) & color.g) << videoMode.greenPosition) |
		((((1<<videoMode.blueMask) - 1) & color.b) << videoMode.bluePosition);
}

extern "C" void video_clear(color_t color)
{
	for(uint32_t y = 0; y < videoMode.res.y; y++) {
		for(uint32_t x = 0; x < videoMode.res.x; x++) {
			set_pixel(x, y, color);
		}
	}
}

extern "C" void * video_buffer()
{
	return (void*)videoMode.framebuffer;
}

extern "C" void video_getmode(videomode_t *mode)
{
	if(mode == nullptr) return;
	mode->width = videoMode.res.x;
	mode->height = videoMode.res.y;
	mode->stride = videoMode.pitch;
	mode->bpp = videoMode.bpp;
}