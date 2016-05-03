#include "numeric.hpp"

static char getDigit(uint32_t i)
{
  if(i >= 0 && i <= 9) {
    return '0' + i;
  }
  return 'A' + (i-10);
}

size_t Numeric::toString(
  char *buffer, 
  size_t length, 
  uint32_t number, 
  uint32_t radix)
{
  if(length == 0) {
    return 0;
  }
  if(number == 0) {
    buffer[0] = '0';
    return 1;
  }

  size_t len = 0;
  while(number > 0)
  {
    buffer[len++] = getDigit(number % radix);
    if(len >= length)
      break;
    number /= radix;
  }
  
  int half = len / 2;
  for(int i = 0; i < half; i++)
  {
    char c = buffer[i];
    buffer[i] = buffer[len - i - 1];
    buffer[len - i - 1] = c;
  }
  
  return len;
}

size_t Numeric::toString(
  char *buffer, 
  size_t length, 
  int32_t number, 
  uint32_t radix)
{
  if(length == 0) {
    return 0;
  }
  if(number == 0) {
    buffer[0] = '0';
    return 1;
  }
  if(number < 0) {
    buffer[0] = '-';
    length -= 1;
    if(length == 0) {
      return 0;
    }
    return Numeric::toString(&buffer[1], length, static_cast<uint32_t>(-number), radix) + 1;
  }
  else {
    return Numeric::toString(buffer, length, static_cast<uint32_t>(number), radix);
  }
}