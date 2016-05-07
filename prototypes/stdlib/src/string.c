#include "string.h"

void * memcpy ( void * destination, const void * source, size_t num )
{
	char *d = destination;
	char const * s = source;
	for(size_t i = 0; i < num; i++) {
		d[i] = s[i];
	}
	return destination;
}

void * memset ( void * ptr, int value, size_t num )
{
	char *p = ptr;
	for(size_t i = 0; i < num; i++) {
		p[i] = value;
	}
	return ptr;
}