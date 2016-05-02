#include "console.hpp"

Console Console::main(&Screen::main);

Console::Console(Screen *screen) : 
  screen(screen),
  x(0), y(0),
  fg(Color::White), bg(Color::Black),
  caretEnabled(true)
{
  this->updateCaret();
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
  this->updateCaret();
}

void Console::newline() {
  this->x = 0;
  this->y += 1;
  
  if(this->y >= this->screen->height) {
    this->scroll();
  }
  this->updateCaret();
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
  this->updateCaret();
}

void Console::setCursor(int x, int y)
{
  if(x < 0)
    x = 0;
  else if(x >= this->screen->width)
    x = this->screen->width - 1;
  if(y < 0)
    y = 0;
  else if(y >= this->screen->height)
    y = this->screen->height - 1;
  this->x = x;
  this->y = y;
  this->updateCaret();
}

void Console::updateCaret()
{
  if(this->caretEnabled) {
    // Move caret to current position.
  } else {
    // Move caret outside the screen.
  }
}

void Console::setCaretVisible(bool visible)
{
  this->caretEnabled = visible;
  this->updateCaret();
}