#pragma once

#include <stddef.h>
#include <stdint.h>

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

namespace utils
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

class CharacterDevice
{
private:
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
	virtual void write(char c) = 0;
  
  inline CharacterDevice & operator << (char c)
  {
    this->write(c);
    return *this;
  }

  inline CharacterDevice & operator << (const char *str)
  {
    while(*str) {
      *this << *str++;
    }
    return *this;
  }
  
  CharacterDevice & operator << (uint32_t value);
  
  CharacterDevice & operator << (int32_t value);
  
  CharacterDevice & operator << (void *value);
  
  CharacterDevice & operator << (bool value);
  
  template<typename T>
  CharacterDevice & operator << (const NumericFormat<T> &fmt);
};