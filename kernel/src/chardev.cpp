#include <chardev.hpp>

#include <numeric.hpp>


CharacterDevice & CharacterDevice::operator << (uint32_t value)
{
  char buffer[12];
  size_t len = Numeric::toString(buffer, sizeof(buffer), value, 10);
  for(size_t i = 0; i < len; i++) {
    this->write(buffer[i]);
  }
  return *this;
}

CharacterDevice & CharacterDevice::operator << (int32_t value)
{
  char buffer[13];
  size_t len = Numeric::toString(buffer, sizeof(buffer), value, 10);
  for(size_t i = 0; i < len; i++) {
    this->write(buffer[i]);
  }
  return *this;
}

CharacterDevice & CharacterDevice::operator << (void *value)
{
  char buffer[13];
  size_t len = Numeric::toString(buffer, sizeof(buffer), reinterpret_cast<uint32_t>(value), 16);
  for(size_t i = 0; i < len; i++) {
    this->write(buffer[i]);
  }
  return *this;
}

CharacterDevice & CharacterDevice::operator << (bool value)
{
  if(value == true) {
    *this << "true";
  } else {
    *this << "false";
  }
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
    this->write(buffer[i]); \
  } \
	if(fmt.padding > 0 && delta < fmt.padding) { \
		this->putrep(fmt.padchar, fmt.padding - delta); \
	} \
  return *this

template<>
CharacterDevice & CharacterDevice::operator << <uint32_t>(const NumericFormat<uint32_t> & fmt)
{
  NUMERIC_FMT_HANDLER;
}

template<>
CharacterDevice & CharacterDevice::operator << <int32_t>(const NumericFormat<int32_t> & fmt)
{
  NUMERIC_FMT_HANDLER;
}

template<>
CharacterDevice & CharacterDevice::operator << <uint64_t>(const NumericFormat<uint64_t> & fmt)
{
  NUMERIC_FMT_HANDLER;
}

template<>
CharacterDevice & CharacterDevice::operator << <int64_t>(const NumericFormat<int64_t> & fmt)
{
  NUMERIC_FMT_HANDLER;
}

uint32_t CharacterDevice::printNumericPrefix(uint32_t base)
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

void CharacterDevice::putrep(char c, uint32_t repetitions)
{
	for(uint32_t i = 0; i < repetitions; i++)
		this->write(c);
}