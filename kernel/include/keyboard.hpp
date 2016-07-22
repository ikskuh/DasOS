#pragma once

#include "syscalls.h"

/**
 * @brief Implementation of SYSCALLS.is_pressed
 */
extern "C" bool kbd_is_pressed(vkey_t key);

/**
 * @brief Implementation of SYSCALLS.getkey
 */
extern "C" bool kbd_getkey(keyhit_t *hit, bool blocking);

/**
 * @brief Implementation of SYSCALLS.getchar
 */
extern "C" codepoint_t kbd_getchar();

/**
 * @brief Initializes the keyboard driver.
 */
void kbd_init(void);