#include "bsod.hpp"

void BSOD::die(Error code, const char *msg)
{
  __asm__ volatile  ("cli");
}