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
  bool caretEnabled;
private:
  /**
   * Moves the hardware caret.
   */ 
  void updateCaret();
public:
	Console(Screen *screen);
  
  /**
   * Clears the console.
   */
   void clear();
	
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
  
  /**
   * Moves the cursor to the given position
   */
  void setCursor(int x, int y);
  
  /**
   * Sets the visibility of the caret.
   */
  void setCaretVisible(bool visible = true);
  
  /**
   * Sets the text foreground color
   */
  void setForeground(Color c) {
    this->fg = c;
  }
  
  /**
   * Sets the text background color
   */
  void setBackground(Color c) {
    this->bg = c;
  }
  
  /**
   * Sets the text colors.
   */
  void setColors(Color bg, Color fg) {
    this->fg = fg;
    this->bg = bg;
  }
  
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
  
  Console & operator << (uint32_t value);
  
  Console & operator << (int32_t value);
  
  Console & operator << (void *value);
};