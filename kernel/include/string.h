#pragma once

#include <stdint.h>
#include <stddef.h>

#if defined(__cplusplus)
extern "C"
{
#endif

void * memset(void *dst, int value, size_t size);
void * memcpy(void *dst, void const * src, size_t size);

char * strcpy(char * dst, const char *src);

char *strcat(char * dst, const char *src);

#if defined(__cplusplus)
}
#endif