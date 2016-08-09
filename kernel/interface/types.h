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

typedef struct
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

typedef enum
{
	VK_Invalid = 0,
	VK_Escape,
	VK_D0, VK_D1, VK_D2,VK_D3, VK_D4, VK_D5, VK_D6, VK_D7, VK_D8, VK_D9,	
	VK_A, VK_B, VK_C, VK_D, VK_E, VK_F, VK_G, VK_H, VK_I, VK_J, VK_K, VK_L, VK_M, VK_N, VK_O, VK_P, VK_Q, VK_R, VK_S, VK_T, VK_U, VK_V, VK_W, VK_X, VK_Y, VK_Z,
	
	// "\\",
	// "`",
	VK_Backspace,
	VK_Tab,
	// "ü",
	VK_Plus,
	VK_Enter,
	VK_ControlLeft,
	// "ö",
	// "ä",
	// "^",
	VK_ShiftLeft,
	VK_Number, // #,
	VK_Comma,
	VK_Dot,
	VK_Hyphen,
	VK_ShiftRight,
	VK_AltLeft,
	VK_Space,
	VK_CapsLock,
	VK_F1, VK_F2, VK_F3, VK_F4, VK_F5, VK_F6, VK_F7, VK_F8, VK_F9, VK_F10, VK_F11, VK_F12,
	VK_ScrollLock,
	VK_NumpadLock,
	VK_NumpadPlus,
	VK_NumpadMinus,
	VK_NumpadMult,
	VK_NumpadDot,
	VK_Numpad0, VK_Numpad1, VK_Numpad2, VK_Numpad3, VK_Numpad4, VK_Numpad5, VK_Numpad6, VK_Numpad7, VK_Numpad8, VK_Numpad9,
	
	VK_LIMIT,
} vkey_t;


/**
 * @brief Key definition on a key map.
 */
typedef struct
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
typedef enum 
{
	KHF_None = 0,
	
	/**
	 * The key was pressed.
	 */
	KHF_KeyPress = (1 << 0),
	
	/**
	 * @brief The key was released
	 */
	KHF_KeyRelease = (1 << 1),
	
	/**
	 * @brief A character was typed with this event.
	 */
	KHF_CharInput = (1 << 2),
} keyhitflags_t;

/**
 * @brief A basic keyboard event
 */
typedef struct
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
typedef struct {
	uint8_t r, g, b, a;
} __attribute__ ((packed)) color_t;

typedef struct {
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