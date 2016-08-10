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
	
	/**
	 * Opens a file and gets a file handle.
	 */
	int (*fs_open)(char const * name);

	/**
	 * Returns the type of the given file.
	 */
	enum filetype (*fs_type)(int file);

	/**
	 * Gets information about a file handle.
	 * @param file The file handle
	 * @param node The target structure where the information will be stored.
	 * @return     true on success, false when error
	 */
	bool (*fs_info)(int file, struct fsnode * node);

	/**
	 * Releases a file handle.
	 */
	void (*fs_close)(int file);

	/**
	 * Reads a block of data from a file.
	 * @param file   The file handle.
	 * @param buffer The target memory
	 * @param offset The start position in the file
	 * @param length Number of bytes to read.
	 * @return       Number of bytes read.
	 */
	uint32_t (*file_read)(
		int file,
		void *buffer,
		uint32_t offset,
		uint32_t length);

	/**
	 * Returns the size of a file in bytes.
	 */
	uint32_t (*file_size)(int file);

	/**
	 * Returns the number of entries in a directory.
	 * @param file The file handle.
	 */
	uint32_t (*dir_length)(int file);

	/**
	 * Gets a directory entry.
	 * @param file  The file handle
	 * @param index The index of the node in the directory. 
	 * @param node  A pointer to a node to store the information.
	 * @return      true if the entry is valid, false if not.
	 * @remarks     If the directory entry does not exist, the node will have the type ftInvalid.
	 */
	bool (*dir_get)(int file, int index, struct fsnode * node);
	
	/**
	 * The file name of the program to execute
	 * @param program Path to the program.
	 */
	void (*exec)(char const *program);
	
	/**
	 * Sets the shell that should be called when a program is ended.
	 * @param program The path of the shell.
	 */
	void (*os_set_shell)(char const * program);
	
	/**
	 * Stops the execution.
	 * @param errorCode An error code that is given to the OS.
	 * @param 
	 */
	void (*exit)(int errorCode);
	
	void (*puts)(const char *text);
	
};

extern struct syscalls SYSCALLS;

#if !defined(DASOS_KERNEL)

static inline codepoint_t getchar() { return SYSCALLS.getchar(); }

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

#define fs_open (SYSCALLS.fs_open)
#define fs_type (SYSCALLS.fs_type)
#define fs_info (SYSCALLS.fs_info)
#define fs_close (SYSCALLS.fs_close)

static inline uint32_t file_size(int fd) { return SYSCALLS.file_size(fd); }
#define file_read (SYSCALLS.file_read)

#define dir_length (SYSCALLS.dir_length)
#define dir_get (SYSCALLS.dir_get)

#define exec (SYSCALLS.exec)
#define os_set_shell (SYSCALLS.os_set_shell)
#define exit (SYSCALLS.exit)

#define puts (SYSCALLS.puts)
#endif

#if defined(__cplusplus)
}
#endif