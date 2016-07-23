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

extern "C" void * video_buffer()
{
	return (void*)0x20000000;
}

static inline void *__movsb(void *d, const void *s, size_t n)
{
	asm volatile ("rep movsb"
		: "=D" (d),
		"=S" (s),
		"=c" (n)
		: "0" (d),
		"1" (s),
		"2" (n)
		: "memory");
	return d;
}

static inline void *__movsw(void *d, const void *s, size_t n)
{
	asm volatile ("rep movsw"
		: "=D" (d),
		"=S" (s),
		"=c" (n)
		: "0" (d),
		"1" (s),
		"2" (n/2)
		: "memory");
	return d;
}

extern "C" void video_swap()
{
	char *src = (char*)video_buffer();
	char *dst = (char*)videoMode.framebuffer;
	uint32_t size = videoMode.pitch * videoMode.res.y;
	__movsw(dst, src, size);
}

extern "C" void video_getmode(videomode_t *mode)
{
	if(mode == nullptr) return;
	mode->width = videoMode.res.x;
	mode->height = videoMode.res.y;
	mode->stride = videoMode.pitch;
	mode->bpp = videoMode.bpp;
}

extern "C" void video_setpixel(uint32_t x, uint32_t y, color_t color)
{
	uint8_t * pixels = (uint8_t*)video_buffer();
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
	
	if(videoMode.bpp == 32) {
		// Just blit the color into the video buffer if we have 32 bpp
		*pixel = *((uint32_t*)(&color));
		if(videoMode.redPosition != 0) {
			// Assert that if we don't have RGBX we will have XBGR
			*pixel = __builtin_bswap32(*pixel) >> 8;
		}
	} else {
		*pixel = 
			(~pixmask & *pixel) |
			((((1<<videoMode.redMask) - 1) & color.r) << videoMode.redPosition) |
			((((1<<videoMode.greenMask) - 1) & color.g) << videoMode.greenPosition) |
			((((1<<videoMode.blueMask) - 1) & color.b) << videoMode.bluePosition);
	}
}

extern "C" void video_clear(color_t color)
{
	for(uint32_t y = 0; y < videoMode.res.y; y++) {
		for(uint32_t x = 0; x < videoMode.res.x; x++) {
			video_setpixel(x, y, color);
		}
	}
}