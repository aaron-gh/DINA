/**
 * @file config.c
 * @brief Configuration implementation for DINA
 *
 * This file handles loading and validating the DINA configuration.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

#include "config.h"
#include "dina.h"
#include "../util/util.h"

/* Include the user configuration */
#include "config.h"

/**
 * @brief Validate a tag number
 * 
 * Ensure tag number is within bounds
 * 
 * @param tag Tag number (0-based)
 * @return 1 if valid, 0 otherwise
 */
static int
validate_tag(int tag)
{
    return (tag >= 0 && tag < LENGTH(tags));
}

/**
 * @brief Initialize configuration
 * 
 * Load and validate the configuration
 */
void
config_init(void)
{
    if (!config_validate()) {
        die("DINA: Invalid configuration");
    }
}

/**
 * @brief Validate configuration
 * 
 * Check that the configuration is valid
 * 
 * @return 1 if valid, 0 otherwise
 */
int
config_validate(void)
{
    int i;
    
    /* Validate fonts */
    if (LENGTH(fonts) < 1) {
        fprintf(stderr, "DINA: No fonts specified\n");
        return 0;
    }
    
    /* Validate tags */
    if (LENGTH(tags) < 1 || LENGTH(tags) > 31) {
        fprintf(stderr, "DINA: Invalid number of tags\n");
        return 0;
    }
    
    /* Validate layouts */
    if (LENGTH(layouts) < 1) {
        fprintf(stderr, "DINA: No layouts specified\n");
        return 0;
    }
    
    /* Validate rules */
    for (i = 0; i < LENGTH(rules); i++) {
        const Rule *r = &rules[i];
        if ((r->tags & TAGMASK) != r->tags) {
            fprintf(stderr, "DINA: Invalid tag mask in rule %d\n", i);
            return 0;
        }
    }
    
    /* Validate keys */
    for (i = 0; i < LENGTH(keys); i++) {
        const Key *k = &keys[i];
        if (!k->func) {
            fprintf(stderr, "DINA: NULL function in key binding %d\n", i);
            return 0;
        }
    }
    
    /* Validate buttons */
    for (i = 0; i < LENGTH(buttons); i++) {
        const Button *b = &buttons[i];
        if (!b->func) {
            fprintf(stderr, "DINA: NULL function in button binding %d\n", i);
            return 0;
        }
    }
    
    return 1;
}

/**
 * @brief Get key binding
 * 
 * Get the key binding for a given keysym and modifiers
 * 
 * @param keysym X11 key symbol
 * @param mod Modifier mask
 * @return Key binding or NULL if not found
 */
const Key *
config_get_key(KeySym keysym, unsigned int mod)
{
    int i;
    
    for (i = 0; i < LENGTH(keys); i++) {
        if (keysym == keys[i].keysym && CLEANMASK(mod) == CLEANMASK(keys[i].mod)) {
            return &keys[i];
        }
    }
    
    return NULL;
}

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
const Button *
config_get_button(unsigned int click, unsigned int button, unsigned int mod)
{
    int i;
    
    for (i = 0; i < LENGTH(buttons); i++) {
        if (click == buttons[i].click && button == buttons[i].button && 
            CLEANMASK(mod) == CLEANMASK(buttons[i].mask)) {
            return &buttons[i];
        }
    }
    
    return NULL;
}