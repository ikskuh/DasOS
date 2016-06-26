#pragma once
#include <stdint.h>

#include <vbe.hpp>

#include "picture.hpp"

class VideoScreen : 
	public Picture
{
private:
	static Pixel outOfScreen;
private:
	Pixel *mFramebuffer;
	uint32_t mPitch;
public:	
	const uint32_t mWidth, mHeight;
public:
	VideoScreen(vbe::ModeInfo const & modeInfo);
	
	uint32_t width() const override {
		return this->mWidth;
	}
	
	uint32_t height() const override {
		return this->mHeight;
	}
	
	Pixel & operator() (uint32_t x, uint32_t y) override {
		if(x >= this->mWidth || y >= this->mHeight) {
			return VideoScreen::outOfScreen;
		} else {
			return this->scanline(y)[x];
		}
	}
	
	Pixel const & operator() (uint32_t x, uint32_t y) const override {
		if(x >= this->mWidth || y >= this->mHeight) {
			return VideoScreen::outOfScreen;
		} else {
			return this->scanline(y)[x];
		}
	}
	
private:
	Pixel * scanline(uint32_t y) {
		return reinterpret_cast<Pixel*>(
			reinterpret_cast<uint32_t>(this->mFramebuffer) + y * this->mPitch
		);
	}
	Pixel const * scanline(uint32_t y) const {
		return reinterpret_cast<Pixel*>(
			reinterpret_cast<uint32_t>(this->mFramebuffer) + y * this->mPitch
		);
	}
};