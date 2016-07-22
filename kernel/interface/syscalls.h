#pragma once

#include <stdbool.h>
#include "types.h"

#if defined(__cplusplus)
extern "C" {
#endif


/**
 * @brief A structure that contains all syscalls of the OS as function pointers.
 */
struct syscalls
{
	/**
	 * @brief Reads a single symbol from the keyboard.
	 * 
	 * This read is blocking and returns a guaranteed value.
	 */
	codepoint_t (*getchar)();
	
	/**
	 * @brief Checks if a given key is pressed.
	 */
	bool (*is_pressed)(vkey_t key);
	
	/**
	 * @brief Gets a key event.
	 *
	 * @param hit      The event structure that should be filled with the event.
	 * @param blocking If true, getkey will wait until a key event is received.
	 * @return         true when a key event was received.
	 */
	bool (*getkey)(keyhit_t *hit, bool blocking);
	
	void (*puts)(const char *text);
};

extern struct syscalls SYSCALLS;

#if !defined(DASOS_KERNEL)
#define getchar (SYSCALLS.getchar)
#define is_pressed (SYSCALLS.is_pressed)
#define getkey (SYSCALLS.getkey)
#define puts (SYSCALLS.puts)
#endif

#if defined(__cplusplus)
}
#endif