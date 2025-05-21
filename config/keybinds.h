/**
 * @file keybinds.h
 * @brief Keybinding configuration for DINA
 *
 * Header file for runtime keybinding configuration
 */

#ifndef __DINA_KEYBINDS_H__
#define __DINA_KEYBINDS_H__

#include <stdbool.h>
#include <X11/keysym.h>

/* Function declarations */
bool parse_keybind(const char *kb_str, DINAKeybind *kb);
void handle_keybind(DINAKeybind *kb);

#endif /* __DINA_KEYBINDS_H__ */