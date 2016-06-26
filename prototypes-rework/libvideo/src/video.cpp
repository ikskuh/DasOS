#include <driver/video.hpp>

Pixel VideoScreen::outOfScreen;

VideoScreen::VideoScreen(vbe::ModeInfo const & modeInfo) : 
	mFramebuffer((Pixel*)modeInfo.framebuffer),
	mPitch(modeInfo.pitch),
	mWidth(modeInfo.res.x),
	mHeight(modeInfo.res.y)
{

}
