#pragma once

#include <stdbool.h>
#include <stdint.h>

/**
 * Maximum length of a file name.
 */ 
#define FILENAME_MAXLEN 13 // 8 + . + 3 + \0

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief A single unicode code point (or character) that stores a text symbol.
 */
typedef uint32_t codepoint_t;

typedef struct memlimits
{
	/**
	 * @brief Begin of userspace memory. Should be 0x40000000
	 */
	uint32_t lower;
	
	/**
	 * @brief End of userspace memory. Memory above this pointer is invalid.
	 */
	uint32_t upper;
} memlimits_t;

typedef enum vkey
{
	vkInvalid = 0,
	vkEscape,
	vkD0, vkD1, vkD2,vkD3, vkD4, vkD5, vkD6, vkD7, vkD8, vkD9,	
	vkA, vkB, vkC, vkD, vkE, vkF, vkG, vkH, vkI, vkJ, vkK, vkL, vkM, vkN, vkO, vkP, vkQ, vkR, vkS, vkT, vkU, vkV, vkW, vkX, vkY, vkZ,
	
	// "\\",
	// "`",
	vkBackspace,
	vkTab,
	// "ü",
	vkPlus,
	vkEnter,
	vkControlLeft,
	// "ö",
	// "ä",
	// "^",
	vkShiftLeft,
	vkNumber, // #,
	vkComma,
	vkDot,
	vkHyphen,
	vkShiftRight,
	vkAltLeft,
	vkSpace,
	vkCapsLock,
	vkF1, vkF2, vkF3, vkF4, vkF5, vkF6, vkF7, vkF8, vkF9, vkF10, vkF11, vkF12,
	vkScrollLock,
	vkNumpadLock,
	vkNumpadPlus,
	vkNumpadMinus,
	vkNumpadMult,
	vkNumpadDot,
	vkNumpad0, vkNumpad1, vkNumpad2, vkNumpad3, vkNumpad4, vkNumpad5, vkNumpad6, vkNumpad7, vkNumpad8, vkNumpad9,
	
	vkLIMIT,
} vkey_t;


/**
 * @brief Key definition on a key map.
 */
typedef struct key
{
	/**
	 * @brief Virtual key code of this scancode.
	 */
	vkey_t key;
	
	/**
	 * Lower case representation.
	 */
	codepoint_t lower;
	
	
	/**
	 * Upper case representation.
	 */
	codepoint_t upper;
	 
	 
	/**
	 * Variant (alt+graph) representation.
	 */
	codepoint_t variant;
} key_t;

/**
 * @brief Defines different flags for key hits.
 */
typedef enum keyhitflags
{
	khfNone = 0,
	
	/**
	 * The key was pressed.
	 */
	khfKeyPress = (1 << 0),
	
	/**
	 * @brief The key was released
	 */
	khfKeyRelease = (1 << 1),
	
	/**
	 * @brief A character was typed with this event.
	 */
	khfCharInput = (1 << 2),
} keyhitflags_t;

/**
 * @brief A basic keyboard event
 */
typedef struct keyhit
{
	/**
	 * @brief The scan code of the key that produces this key hit.
	 */
	struct {
		uint16_t code;
		int16_t set;
	} scanCode;
	
	/**
	 * @brief The key that was hit.
	 */
	key_t key;
	
	/**
	 * @brief The code point (character) value that is associated with
	 *        the pressed key.
	 */
	codepoint_t codepoint;
	
	/**
	 * @brief Flags that define the specific type and behaviour of this key hit.
	 */
	keyhitflags_t flags;
} keyhit_t;

/**
 * @brief RGBA color value.
 */
typedef struct color 
{
	uint8_t r, g, b, a;
} __attribute__ ((packed)) color_t;

typedef struct videomode
{
	/**
	 * @brief Size of the screen.
	 */
	uint32_t width, height;
	
	/**
	 * @brief Length in bytes of a single scan line.
	 */
	uint32_t stride;
	
	/**
	 * @brief Bits per pixel
	 */
	uint32_t bpp;
} videomode_t;

/**
 * An enum listing all possible file types.
 */
typedef enum filetype
{
	/**
	 * The file type is not known to the system and may be supported later.
	 */
	ftUnknown = 0, 
	/**
	 * The file is a simple data file.
	 */
	ftFile = 1, 
	/**
	 * The file is a directory.
	 */
	ftDirectory = 2,
	/**
	 * The file is not a valid file.
	 */
	ftInvalid = 99,
} filetype_t;

/**
 * A node of the file system.
 */
typedef struct fsnode
{
	/**
	 * The name of the file
	 */
	char name[FILENAME_MAXLEN];
	
	/**
	 * The type of the file
	 */
	enum filetype type;
} fsnode_t;

#if defined(__cplusplus)
}
#endif