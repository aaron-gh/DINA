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

/* External variables from dina.c */
extern Display *dpy;
extern Window root;
extern unsigned int numlockmask;

/* Global definitions from config.h */
const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

/* Define TAGMASK (all valid tags) */
#define TAGMASK ((1 << LENGTH(tags)) - 1)

/* Define BUTTONMASK (button events) */
#define BUTTONMASK  (ButtonPressMask|ButtonReleaseMask)

/* Define MOUSEMASK (mouse events) */
#define MOUSEMASK   (BUTTONMASK|PointerMotionMask)

/* User configuration is already included via config.h */

/**
 * @brief Clean modifier mask
 * 
 * Remove numlock and capslock masks from modifiers
 * 
 * @param mask Modifier mask
 * @return Cleaned mask
 */
unsigned int
cleanmask(unsigned int mask)
{
    return (mask & ~(numlockmask|LockMask) & (ShiftMask|ControlMask|Mod1Mask|Mod2Mask|Mod3Mask|Mod4Mask|Mod5Mask));
}

/* Define CLEANMASK as a macro that calls our cleanmask function */
#undef CLEANMASK
#define CLEANMASK(mask) cleanmask(mask)

/**
 * @brief Update the numlock mask
 * 
 * Determine which modifier bit corresponds to numlock
 */
void
updatenumlockmask(void)
{
    unsigned int i, j;
    XModifierKeymap *modmap;

    numlockmask = 0;
    modmap = XGetModifierMapping(dpy);
    for (i = 0; i < 8; i++)
        for (j = 0; j < (unsigned int)modmap->max_keypermod; j++)
            if (modmap->modifiermap[i * modmap->max_keypermod + j]
                == XKeysymToKeycode(dpy, XK_Num_Lock))
                numlockmask = (1 << i);
    XFreeModifiermap(modmap);
}

/**
 * @brief Grab keyboard buttons for a client
 * 
 * Set up button grabs for a client window
 * 
 * @param c Client to grab buttons for
 * @param focused Whether the client is focused
 */
void
grabbuttons(Client *c, int focused)
{
    updatenumlockmask();
    {
        unsigned int i, j;
        unsigned int modifiers[] = { 0, LockMask, numlockmask, numlockmask|LockMask };
        XUngrabButton(dpy, AnyButton, AnyModifier, c->win);
        if (!focused)
            XGrabButton(dpy, AnyButton, AnyModifier, c->win, False,
                BUTTONMASK, GrabModeSync, GrabModeSync, None, None);
        for (i = 0; i < LENGTH(buttons); i++)
            if (buttons[i].click == ClkClientWin)
                for (j = 0; j < LENGTH(modifiers); j++)
                    XGrabButton(dpy, buttons[i].button,
                        buttons[i].mask | modifiers[j],
                        c->win, False, BUTTONMASK,
                        GrabModeAsync, GrabModeSync, None, None);
    }
}

/**
 * @brief Grab keyboard keys
 * 
 * Set up key grabs for the root window
 */
void
grabkeys(void)
{
    updatenumlockmask();
    {
        unsigned int i, j;
        unsigned int k;
        unsigned int modifiers[] = { 0, LockMask, numlockmask, numlockmask|LockMask };
        int start, end, skip;
        KeySym *syms;

        XUngrabKey(dpy, AnyKey, AnyModifier, root);
        XDisplayKeycodes(dpy, &start, &end);
        syms = XGetKeyboardMapping(dpy, start, end - start + 1, &skip);
        if (!syms)
            return;
        for (k = (unsigned int)start; k <= (unsigned int)end; k++)
            for (i = 0; i < LENGTH(keys); i++)
                /* skip modifier codes, we do that ourselves */
                if (keys[i].keysym == syms[(k - (unsigned int)start) * skip])
                    for (j = 0; j < LENGTH(modifiers); j++)
                        XGrabKey(dpy, k,
                            keys[i].mod | modifiers[j],
                            root, True,
                            GrabModeAsync, GrabModeAsync);
        XFree(syms);
    }
}

/**
 * @brief Validate a tag number
 * 
 * Ensure tag number is within bounds
 * 
 * @param tag Tag number (0-based)
 * @return 1 if valid, 0 otherwise
 */
__attribute__((unused)) static int
validate_tag(unsigned int tag)
{
    return (tag < LENGTH(tags));
}

/**
 * @brief Initialize configuration
 * 
 * Load and validate the configuration
 */
int config_validate(void); /* Forward declaration */

void
config_init(void)
{
    if (!config_validate()) {
        die("DINA: Invalid configuration");
    }
    
    /* Set up keyboard and mouse bindings */
    grabkeys();
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
    unsigned int i;
    
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
    unsigned int i;
    
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
    unsigned int i;
    
    for (i = 0; i < LENGTH(buttons); i++) {
        if (click == buttons[i].click && button == buttons[i].button && 
            CLEANMASK(mod) == CLEANMASK(buttons[i].mask)) {
            return &buttons[i];
        }
    }
    
    return NULL;
}