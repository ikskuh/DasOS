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
	 * @brief Gets the memory limits for the current platform.
	 * @param limits Pointer to the result.
	 */
	void (*get_memlimits)(memlimits_t *limits);

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
	
	/**
	 * @brief Clears the screen to a given color.
	 */
	void (*video_clear)(color_t color);

	/**
	 * @brief Gets a pointer to the current frame buffer.
	 */
	void * (*video_buffer)();

	/**
	 * @brief Gets the current video mode.
	 * @param mode A pointer to the result.
	 */
	void (*video_getmode)(videomode_t *mode);
	
	/**
	 * @brief Sets a pixel.
	 * @param x x coordinate
	 * @param y y coordinate
	 * @param c pixel color
	 */
	void (*video_setpixel)(uint32_t x, uint32_t y, color_t c);
	
	/**
	 * @brief Swaps the front and the back buffer.
	 *
	 * Presents the current back buffer on the screen.
	 */
	void (*video_swap)();
	
	/**
	 * @brief Gets the current timer counter.
	 * @return The current counter.
	 */
	uint32_t (*timer_get)();
	
	/**
	 * @brief Resets the timer counter to zero.
	 */
	void (*timer_reset)();
	
	void (*puts)(const char *text);
};

extern struct syscalls SYSCALLS;

#if !defined(DASOS_KERNEL)

#define getchar (SYSCALLS.getchar)
#define is_pressed (SYSCALLS.is_pressed)
#define getkey (SYSCALLS.getkey)

#define video_clear (SYSCALLS.video_clear)
#define video_buffer (SYSCALLS.video_buffer)
#define video_getmode (SYSCALLS.video_getmode)
#define video_setpixel (SYSCALLS.video_setpixel)
#define video_swap (SYSCALLS.video_swap)

#define timer_get (SYSCALLS.timer_get)
#define timer_reset (SYSCALLS.timer_reset)

#define puts (SYSCALLS.puts)
#endif

#if defined(__cplusplus)
}
#endif