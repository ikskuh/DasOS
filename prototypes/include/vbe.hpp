#pragma once

#include <stddef.h>

#define VBE_FAR(name) uint32_t name;

namespace vbe
{
	struct ModeInfo
	{
		uint16_t attributes;
		uint8_t winA,winB;
		uint16_t granularity;
		uint16_t winsize;
		uint16_t segmentA, segmentB;
		VBE_FAR(realFctPtr);
		uint16_t pitch; // bytes per scanline

		struct {
			uint16_t x;
			uint16_t y;
		} __attribute__ ((packed)) res;
		uint8_t Wchar, Ychar;
		uint8_t planes, bpp, banks;
		uint8_t memoryModel, bankSize, imagePages;
		uint8_t reserved0;

		uint8_t redMask, redPosition;
		uint8_t greenMask, greenPosition;
		uint8_t blueMask, bluePosition;
		uint8_t rsvMask, rsvPosition;
		uint8_t directcolor_attributes;

		uint32_t framebuffer;
		uint32_t reserved1;
		uint16_t reserved2;
	} __attribute__((packed));
	
}