#pragma once

#include "screen.hpp"

class Console
{
public:
  static Console main;
private:
	Screen * const screen;
  int x;
	int y;
public:
	Console(Screen *screen);
	
	void put(char c);
	
	void newline();
};

static inline Console operator << (Console & console, char c) {
  console.put(c);
  return console;
}

static inline Console operator << (Console & console, const char *str) {
  while(*str) {
    console << *str++;
  }
  return console;
}