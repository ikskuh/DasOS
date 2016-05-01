#pragma once

#include <inttypes.h>

enum class Color : uint8_t
{
  Black = 0,
  Blue = 1,
  Green = 2,
  Cyan = 3,
  Red = 4,
  Magenta = 5,
  Brown = 6,
  LightGray = 7,
  Gray = 8,
  LightBlue = 9,
  LightGreen = 10,
  LightCyan = 11,
  LightRed = 12,
  LightMagenta = 13,
  Yellow = 14,
  White = 15,
};

using ColorInteger = uint8_t;

struct ScreenChar
{
  char c;
  struct {
    ColorInteger fg : 4;
    ColorInteger bg : 4;
  };
};

class Screen
{
private:
  static ScreenChar outOfScreen;
private:
  ScreenChar * const buffer;
  const int width;
  const int height;
public:
  static Screen main;
public:
  Screen(ScreenChar *buffer, int width, int height);
  
  void clear();
  
  ScreenChar & operator ()(int x, int y) {
    if(x < 0 || y < 0 || x >= this->width || y >= this->height) {
      // return Screen::outOfScreen;
    }
    return this->buffer[this->width * y + x];
  }
  ScreenChar const & operator ()(int x, int y) const {
    if(x < 0 || y < 0 || x >= this->width || y >= this->height) {
      return Screen::outOfScreen;
    }
    return this->buffer[this->width * y + x];
  }
};