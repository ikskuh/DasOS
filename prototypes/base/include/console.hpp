#pragma once

#include "screen.hpp"

struct FColor
{
  FColor() : color(Color::White) { }
  FColor(Color color) : color(color) { }
  Color color;
};

struct BColor
{
  BColor() : color(Color::Black) { }
  BColor(Color color) : color(color) { }
  Color color;
};

class Console
{
public:
  static Console main;
private:
	Screen * const screen;
  int x;
	int y;
  Color fg, bg;
public:
	Console(Screen *screen);
	
  /**
   * Puts a character on the screen.
   */
	void put(char c);
	
  /**
   * Inserts a line break and returns the cursor to the start.
   */
	void newline();
  
  /**
   * Scrolls screen a line upwards
   */
  void scroll();
  
  inline Console & operator << (char c)
  {
    this->put(c);
    return *this;
  }

  inline Console & operator << (const char *str)
  {
    while(*str) {
      *this << *str++;
    }
    return *this;
  }
  
  inline Console & operator << (const FColor &color)
  {
    this->fg = color.color;
    return *this;
  }
  
  inline Console & operator << (const BColor &color)
  {
    this->bg = color.color;
    return *this;
  }
};