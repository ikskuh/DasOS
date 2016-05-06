#pragma once

#include "screen.hpp"
#include "pointer.hpp"

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

template<typename T>
struct NumericFormat
{
  T value;
  uint32_t base;
	
	/**
	 * Applies a padding to the number.
	 * A positive number will apply padding the right side,
	 * a negative number will pad the left side.
	 */
	int32_t padding = 0;
	
	char padchar = ' ';

  NumericFormat(T value) : value(value), base(10) { }
  NumericFormat(T value, uint32_t base) : value(value), base(base) { }
};

namespace console_tools
{
  template<typename T>
  auto hex(T value) { return NumericFormat<T>(value, 16); }
  
  template<typename T>
  auto dec(T value) { return NumericFormat<T>(value, 10); }
  
  template<typename T>
  auto oct(T value) { return NumericFormat<T>(value, 8); }
  
  template<typename T>
  auto bin(T value) { return NumericFormat<T>(value, 2); }
  
  template<typename T>
  auto nbase(T value, uint32_t base) { return NumericFormat<T>(value, base); }
	
	template<typename T>
	auto pad(NumericFormat<T> value, int32_t padding, char c = ' ') { 
		value.padding = padding;
		return value;
	}
	
	template<typename T>
	auto pad(T value, int32_t padding, char c = ' ') { 
		return pad(NumericFormat<T>(value), padding, c);
	}
}

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
	
	/**
	 * Prints the prefix for a given numeric base.
	 * @returns the prefix length.
	 */
	uint32_t printNumericPrefix(uint32_t base);
	
	/**
	 * Prints a character several times.
	 */
	void putrep(char c, uint32_t repetitions);
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
  
  Console & operator << (bool value);
  
  template<typename T>
  Console & operator << (pointer<T> ptr);
  
  template<typename T>
  Console & operator << (const NumericFormat<T> &fmt);
};