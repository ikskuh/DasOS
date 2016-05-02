#include "console.hpp"

Console Console::main(&Screen::main);

Console::Console(Screen *screen) : 
  screen(screen),
  x(0), y(0)
{
  
}

void Console::put(char c)
{
  switch(c) {
  
    case '\r': break; /* ignore \r */
    case '\n':
      this->newline();
      break;
    default:
      ScreenChar &sc = (*this->screen)(this->x++, this->y);
      sc.c = c;
      sc.fg = (int)Color::Black;
      sc.bg = (int)Color::LightRed;
      break;
  }
  if(this->x >= this->screen->width) {
    this->newline();
  }
}

void Console::newline() {
  this->x = 0;
  this->y += 1;
}