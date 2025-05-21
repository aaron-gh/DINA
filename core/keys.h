/**
 * @file keys.h
 * @brief Keybinding system for DINA
 *
 * This file defines the keybinding system for DINA with support for
 * multi-layered keybindings and runtime configuration.
 */

#ifndef __DINA_KEYS_H__
#define __DINA_KEYS_H__

#include <stdbool.h>
#include <X11/keysym.h>
#include <X11/X.h>
#include "dina.h"

/* Key mode for multi-layered keybindings */
typedef enum {
    MODE_NORMAL = 0,  // Normal mode (default)
    MODE_COMMAND,     // Command mode 
    MODE_WINDOW,      // Window management mode
    MODE_TAG,         // Tag/workspace mode
    
    // Add more modes as needed
    MODE_LAST         // Must be last
} KeyMode;

/* Advanced keybinding structure */
typedef struct {
    unsigned int mod;      // Modifiers (e.g., Mod4Mask)
    KeySym keysym;         // Key symbol (e.g., XK_a)
    KeyMode mode;          // Key mode
    void (*func)(const Arg *); // Function to call
    Arg arg;               // Argument to pass to function
    char *exec;            // Command to execute (if not NULL)
} AdvancedKey;

/* Function declarations */
void keys_init(void);
void keys_cleanup(void);
void handle_keypress(XEvent *e);
bool register_keybinding(unsigned int mod, KeySym keysym, KeyMode mode, 
                          void (*func)(const Arg *), const Arg *arg, const char *exec);
void unregister_keybinding(unsigned int mod, KeySym keysym, KeyMode mode);
void set_key_mode(KeyMode mode);
void set_key_mode_arg(const Arg *arg);
KeyMode get_key_mode(void);
void reset_key_mode(void);
void execute_command(const char *cmd);
void start_orca(void);
void grab_all_keys(void);

/* Convenience macros for keybinding configuration */
#define KEY_NORMAL(MOD, KEY, FUNC, ARG) \
    { (MOD), (KEY), MODE_NORMAL, (FUNC), (ARG), NULL }

#define KEY_COMMAND(MOD, KEY, FUNC, ARG) \
    { (MOD), (KEY), MODE_COMMAND, (FUNC), (ARG), NULL }

#define KEY_EXEC(MOD, KEY, CMD) \
    { (MOD), (KEY), MODE_NORMAL, NULL, {0}, (CMD) }

#endif /* __DINA_KEYS_H__ */