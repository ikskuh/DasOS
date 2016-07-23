#pragma once

#include "vbe.hpp"
#include "syscalls.h"

extern vbe::ModeInfo videoMode;


void vid_init(vbe::ModeInfo const * videoMode);

extern "C" void video_clear(color_t color);

extern "C" void * video_buffer();

extern "C" void video_getmode(videomode_t *);

extern "C" void video_setpixel(uint32_t x, uint32_t y, color_t c);

extern "C" void video_swap();