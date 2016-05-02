#include "console.hpp"

Console Console::main(&Screen::main);

Console::Console(Screen *screen) : 
  screen(screen),
  x(0), y(0),
  fg(Color::White), bg(Color::Black)
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
      sc.fg = (int)this->fg;
      sc.bg = (int)this->bg;
      break;
  }
  if(this->x >= this->screen->height) {
    this->newline();
  }
}

void Console::newline() {
  this->x = 0;
  this->y += 1;
  
  if(this->y >= this->screen->height) {
    this->scroll();
  }
}

void Console::scroll() {
  if(this->y <= 0) {
    return;
  }
  for(int i = 0; i <= this->y; i++) {
    for(int j = 0; j < this->screen->width; j++) {
      (*this->screen)(j, i) = (*this->screen)(j, i + 1);
    }
  }
  this->y -= 1;
}