#pragma once

#include <stddef.h>

#if defined(__cplusplus)
extern "C" { 
#endif

void * memcpy ( void * destination, const void * source, size_t num );

void * memset ( void * ptr, int value, size_t num );


#if defined(__cplusplus)
}
#endif