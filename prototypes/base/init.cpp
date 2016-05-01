#include <inttypes.h>

#include "screen.hpp"
#include "compat.h"

extern "C" void init(void)
{
  const char hw[] = "Hello World!";
  int i;

  // C-Strings haben ein Nullbyte als Abschluss
  for (i = 0; hw[i] != '\0'; i++) {

    // Zeichen i in den Videospeicher kopieren
    Screen::main(i,0).c = hw[i];

    // 0x07 = Hellgrau auf Schwarz
    Screen::main(i,0).fg = (int)Color::Black;
    Screen::main(i,0).bg = (int)Color::LightRed;
  }
}
