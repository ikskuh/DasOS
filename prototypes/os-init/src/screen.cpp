#include <stdint.h>
#include <screen.hpp>

ScreenChar Screen::outOfScreen;
Screen Screen::main((ScreenChar*)0xb8000, 80, 25);

Screen::Screen(ScreenChar *buffer, int width, int height) : 
  buffer(buffer),
  width(width),
  height(height)
{
  this->clear();
}

void Screen::clear() {
  for(int i = 0; i < this->width * this->height; i++) {
    this->buffer[i].c = ' ';
  }
}
