#pragma once
#include <stdint.h>

struct Pixel
{
	uint8_t b;
	uint8_t g;
	uint8_t r;
	uint8_t a;
} __attribute__((packed));

static inline Pixel COLOR(uint8_t r, uint8_t g, uint8_t b)
{
	return Pixel { b, g, r, 0xFF };
}

class Picture
{
public:
	virtual uint32_t width() const = 0;
	virtual uint32_t height() const = 0;

	virtual Pixel & operator() (uint32_t x, uint32_t y) = 0;
	virtual Pixel const & operator() (uint32_t x, uint32_t y) const = 0;
};