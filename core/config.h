/**
 * @file config.h
 * @brief Configuration interface for DINA
 *
 * This file contains functions for loading and validating the DINA configuration.
 */

#ifndef _CONFIG_H
#define _CONFIG_H

#include "core/dina.h"

/**
 * @brief Initialize configuration
 * 
 * Load and validate the configuration
 */
void config_init(void);

/**
 * @brief Validate configuration
 * 
 * Check that the configuration is valid
 * 
 * @return 1 if valid, 0 otherwise
 */
int config_validate(void);

/**
 * @brief Get key binding
 * 
 * Get the key binding for a given keysym and modifiers
 * 
 * @param keysym X11 key symbol
 * @param mod Modifier mask
 * @return Key binding or NULL if not found
 */
const Key *config_get_key(KeySym keysym, unsigned int mod);

/**
 * @brief Get button binding
 * 
 * Get the button binding for a given click type, button, and modifiers
 * 
 * @param click Button click type
 * @param button Button number
 * @param mod Modifier mask
 * @return Button binding or NULL if not found
 */
const Button *config_get_button(unsigned int click, unsigned int button, unsigned int mod);

/* External configuration variables */
extern const char *fonts[];
extern const float mfact;
extern const int nmaster;
extern const int resizehints;
extern const int lockfullscreen;
extern const char *tags[];
extern const Layout layouts[];
extern const unsigned int borderpx;
extern const Key keys[];
extern const Button buttons[];
extern const Rule rules[];

#endif /* _CONFIG_H */