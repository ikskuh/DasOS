#include "console.hpp"
#include "numeric.hpp"
#include "io.hpp"

Console Console::main(&Screen::main);

Console::Console(Screen *screen) : 
  screen(screen),
  x(0), y(0),
  fg(Color::White), bg(Color::Black),
  caretEnabled(true)
{
  this->updateCaret();
}

void Console::clear()
{
  for(int y = 0; y < this->screen->height; y++) {
    for(int x = 0; x < this->screen->width; x++) {
      auto &c = (*this->screen)(x, y);
      c.c = ' ';
      c.fg = (int)this->fg;
      c.bg = (int)this->bg;
    }
  }
  this->x = 0;
  this->y = 0;
  this->updateCaret();
}

// Prüft, ob man bereits schreiben kann
static uint8_t is_transmit_empty(uint16_t base) {
	return inb(base+5) & 0x20;
}
 
 // Byte senden
static void write_com(uint16_t base, uint8_t chr) {
	while (is_transmit_empty(base)==0);
	outb(base,chr);
}

void Console::put(char c)
{
	write_com(0x3F8, c);
  switch(c) {
    case '\0': return;
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
  if(this->x >= this->screen->width) {
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
  uint16_t tmp;
  if(this->caretEnabled) {
    tmp = this->y * this->screen->width + this->x;
  } else {
    tmp = this->screen->height * this->screen->width;
  }
  outb(0x3D4, 14);
  outb(0x3D5, tmp >> 8);
  outb(0x3D4, 15);
  outb(0x3D5, tmp);
}

void Console::setCaretVisible(bool visible)
{
  this->caretEnabled = visible;
  this->updateCaret();
}



Console & Console::operator << (uint32_t value)
{
  char buffer[12];
  size_t len = Numeric::toString(buffer, sizeof(buffer), value, 10);
  for(size_t i = 0; i < len; i++) {
    this->put(buffer[i]);
  }
  return *this;
}

Console & Console::operator << (int32_t value)
{
  char buffer[13];
  size_t len = Numeric::toString(buffer, sizeof(buffer), value, 10);
  for(size_t i = 0; i < len; i++) {
    this->put(buffer[i]);
  }
  return *this;
}

Console & Console::operator << (void *value)
{
  char buffer[13];
  size_t len = Numeric::toString(buffer, sizeof(buffer), reinterpret_cast<uint32_t>(value), 16);
  for(size_t i = 0; i < len; i++) {
    this->put(buffer[i]);
  }
  return *this;
}

Console & Console::operator << (bool value)
{
  if(value == true) {
    *this << "true";
  } else {
    *this << "false";
  }
  return *this;
}


template<>
Console & Console::operator << <physical_t_ident>(physical_t ptr) 
{
  *this << "physical(0x" << ptr.data() << ")";
  return *this;
}

template<>
Console & Console::operator << <virtual_t_ident>(virtual_t ptr) 
{
  *this << "virtual(0x" << ptr.data() << ")";
  return *this;
}

#define NUMERIC_FMT_HANDLER char buffer[13]; \
	size_t prefixlen = this->printNumericPrefix(fmt.base); \
  size_t len = Numeric::toString(buffer, sizeof(buffer), fmt.value, fmt.base); \
	int delta = prefixlen + len; \
	if(fmt.padding < 0 && delta < -fmt.padding) { \
		this->putrep(fmt.padchar, -fmt.padding - delta); \
	} \
  for(size_t i = 0; i < len; i++) { \
    this->put(buffer[i]); \
  } \
	if(fmt.padding > 0 && delta < fmt.padding) { \
		this->putrep(fmt.padchar, fmt.padding - delta); \
	} \
  return *this

template<>
Console & Console::operator << <uint32_t>(const NumericFormat<uint32_t> & fmt)
{
  NUMERIC_FMT_HANDLER;
}

template<>
Console & Console::operator << <int32_t>(const NumericFormat<int32_t> & fmt)
{
  NUMERIC_FMT_HANDLER;
}

template<>
Console & Console::operator << <uint64_t>(const NumericFormat<uint64_t> & fmt)
{
  NUMERIC_FMT_HANDLER;
}

template<>
Console & Console::operator << <int64_t>(const NumericFormat<int64_t> & fmt)
{
  NUMERIC_FMT_HANDLER;
}

uint32_t Console::printNumericPrefix(uint32_t base)
{
	switch(base) {
		case 2: *this << "0b"; return 2;
		case 8: *this << "0o"; return 2;
		case 10: return 0;
		case 16: *this << "0x"; return 2;
		default:
			*this << "[" << base << "]x";
			if(base < 10) return 4;
			if(base < 100) return 5;
			return 6;
	}
}

void Console::putrep(char c, uint32_t repetitions)
{
	for(uint32_t i = 0; i < repetitions; i++)
		this->put(c);
}