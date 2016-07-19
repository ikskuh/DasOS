
#include "string.h"


void * memset(void *dst, int value, size_t size)
{
	char *ptr = dst;
	while(size--) {
		*ptr++ = value;
	}
	return dst;
}

void * memcpy(void *dst, void const * src, size_t size)
{
	char *ptr = dst;
	char const * s = src;
	while(size--) {
		*ptr++ = *s++;
	}
	return dst;
}

char * strcpy(char * dst, const char *src)
{
    char * tmp = dst;
    while(((*tmp++) = (*src++)));
    return dst;
}

char *strcat(char * dst, const char *src)
{
    char * tmp = dst;
    while(*tmp++);
    tmp--;
    while(((*tmp++) = (*src++)));
    return dst;
}
