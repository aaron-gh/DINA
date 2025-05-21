/**
 * @file keys.c
 * @brief Keybinding system implementation for DINA
 *
 * This module implements the keybinding system with support for
 * multi-layered keybindings and runtime configuration.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

#include "keys.h"
#include "dina.h"
#include "event.h"
#include "config.h"
#include "../a11y/notify.h"
#include "../util/util.h"

/* External variables from dina.c */
extern Display *dpy;
extern Window root;
extern unsigned int numlockmask;

/* External variables from config.c */
extern const Key keys[];

/* Static variables */
static KeyMode current_mode = MODE_NORMAL;
static AdvancedKey *advanced_keys = NULL;
static unsigned int num_advanced_keys = 0;

/* Forward declarations */
void grab_all_keys(void);
static void register_config_keys(void);

/**
 * @brief Initialize keybinding system
 */
void
keys_init(void)
{
    /* Register standard keybindings from config */
    register_config_keys();
    
    /* Register additional default keybindings */
    register_keybinding(Mod4Mask, XK_Escape, MODE_NORMAL, NULL, NULL, NULL);
    register_keybinding(Mod4Mask, XK_Return, MODE_NORMAL, NULL, NULL, "alacritty");
    register_keybinding(Mod4Mask, XK_a, MODE_NORMAL, NULL, NULL, "~/.local/bin/app-launcher");
    register_keybinding(Mod4Mask, XK_b, MODE_NORMAL, NULL, NULL, "~/.local/bin/toggle-blackout");
    register_keybinding(Mod4Mask, XK_BackSpace, MODE_NORMAL, NULL, NULL, "~/.local/bin/session-menu");
    
    /* Mode switching keys */
    Arg cmdmode = {.i = MODE_COMMAND};
    Arg winmode = {.i = MODE_WINDOW};
    Arg tagmode = {.i = MODE_TAG};
    register_keybinding(Mod4Mask, XK_c, MODE_NORMAL, set_key_mode_arg, &cmdmode, NULL);
    register_keybinding(Mod4Mask, XK_w, MODE_NORMAL, set_key_mode_arg, &winmode, NULL);
    register_keybinding(Mod4Mask, XK_t, MODE_NORMAL, set_key_mode_arg, &tagmode, NULL);
    
    /* Command mode keys */
    register_keybinding(Mod4Mask, XK_q, MODE_COMMAND, NULL, NULL, "spd-say -r -40 \"Command: q is pressed\"");
    register_keybinding(Mod4Mask, XK_w, MODE_COMMAND, NULL, NULL, "spd-say -r -40 \"Command: w is pressed\"");
    register_keybinding(Mod4Mask, XK_e, MODE_COMMAND, NULL, NULL, "spd-say -r -40 \"Command: e is pressed\"");
    
    /* Window mode keys */
    register_keybinding(Mod4Mask, XK_h, MODE_WINDOW, NULL, NULL, "spd-say -r -40 \"Window: Move left\"");
    register_keybinding(Mod4Mask, XK_j, MODE_WINDOW, NULL, NULL, "spd-say -r -40 \"Window: Move down\"");
    register_keybinding(Mod4Mask, XK_k, MODE_WINDOW, NULL, NULL, "spd-say -r -40 \"Window: Move up\"");
    register_keybinding(Mod4Mask, XK_l, MODE_WINDOW, NULL, NULL, "spd-say -r -40 \"Window: Move right\"");
    
    /* Tag mode keys */
    register_keybinding(Mod4Mask, XK_1, MODE_TAG, NULL, NULL, "spd-say -r -40 \"Tag: Switch to workspace 1\"");
    register_keybinding(Mod4Mask, XK_2, MODE_TAG, NULL, NULL, "spd-say -r -40 \"Tag: Switch to workspace 2\"");
    register_keybinding(Mod4Mask, XK_3, MODE_TAG, NULL, NULL, "spd-say -r -40 \"Tag: Switch to workspace 3\"");
    
    /* Grab all registered keys */
    grab_all_keys();
}

/**
 * @brief Clean up keybinding system
 */
void
keys_cleanup(void)
{
    unsigned int i;
    
    /* Free allocated memory for exec commands */
    for (i = 0; i < num_advanced_keys; i++) {
        if (advanced_keys[i].exec != NULL) {
            free(advanced_keys[i].exec);
        }
    }
    
    /* Free advanced keys array */
    free(advanced_keys);
    advanced_keys = NULL;
    num_advanced_keys = 0;
}

/**
 * @brief Set current key mode
 * 
 * @param mode Key mode to set
 */
/* Helper function to set key mode through an Arg */
void
set_key_mode_arg(const Arg *arg)
{
    if (arg && arg->i >= 0 && arg->i < MODE_LAST) {
        set_key_mode((KeyMode)arg->i);
    }
}

void
set_key_mode(KeyMode mode)
{
    if (mode >= MODE_LAST) {
        fprintf(stderr, "DINA: Invalid key mode: %d\n", mode);
        return;
    }
    
    /* Only update mode if it's different */
    if (current_mode != mode) {
        current_mode = mode;
        
        /* Play sound/announce mode change */
        char cmd[256];
        const char *mode_name = "normal";
        
        switch (mode) {
            case MODE_NORMAL:
                mode_name = "normal";
                break;
            case MODE_COMMAND:
                mode_name = "command";
                break;
            case MODE_WINDOW:
                mode_name = "window";
                break;
            case MODE_TAG:
                mode_name = "tag";
                break;
            default:
                mode_name = "unknown";
        }
        
        snprintf(cmd, sizeof(cmd), 
                 "play -nq synth 0.1 sine 800 vol 0.3 & spd-say -r -40 \"Mode %s\"",
                 mode_name);
        
        /* Execute command */
        if (system(cmd) == -1) {
            fprintf(stderr, "DINA: Failed to execute command: %s\n", cmd);
        }
        
        /* Regrab keys for the new mode */
        grab_all_keys();
    }
}

/**
 * @brief Get current key mode
 * 
 * @return Current key mode
 */
KeyMode
get_key_mode(void)
{
    return current_mode;
}

/**
 * @brief Reset key mode to normal
 */
void
reset_key_mode(void)
{
    set_key_mode(MODE_NORMAL);
}

/**
 * @brief Handle key press event
 * 
 * @param e X event
 */
void
handle_keypress(XEvent *e)
{
    unsigned int i;
    KeySym keysym;
    XKeyEvent *ev;
    bool key_handled = false;

    ev = &e->xkey;
    keysym = XKeycodeToKeysym(dpy, (KeyCode)ev->keycode, 0);
    
    /* First check if Escape is pressed to reset key mode */
    if (keysym == XK_Escape && current_mode != MODE_NORMAL) {
        reset_key_mode();
        return;
    }
    
    /* Check advanced keybindings */
    for (i = 0; i < num_advanced_keys; i++) {
        if (keysym == advanced_keys[i].keysym
            && CLEANMASK(advanced_keys[i].mod) == CLEANMASK(ev->state)
            && advanced_keys[i].mode == current_mode) {
            
            /* Key found, handle it */
            key_handled = true;
            
            /* If it's an exec command, execute it */
            if (advanced_keys[i].exec != NULL) {
                execute_command(advanced_keys[i].exec);
            } 
            /* If it's a function, call it */
            else if (advanced_keys[i].func != NULL) {
                advanced_keys[i].func(&advanced_keys[i].arg);
            }
            
            break;
        }
    }
    
    /* If no advanced key found, check original key bindings */
    if (!key_handled && current_mode == MODE_NORMAL) {
        for (i = 0; i < LENGTH(keys); i++) {
            if (keysym == keys[i].keysym
                && CLEANMASK(keys[i].mod) == CLEANMASK(ev->state)
                && keys[i].func) {
                
                keys[i].func(&(keys[i].arg));
                key_handled = true;
                break;
            }
        }
    }
}

/**
 * @brief Execute a shell command
 * 
 * @param cmd Command to execute
 */
void
execute_command(const char *cmd)
{
    if (!cmd || !*cmd)
        return;
    
    /* Fork and execute command in background */
    if (fork() == 0) {
        if (fork() == 0) {
            /* Grandchild process to prevent zombies */
            setsid();
            
            /* Close standard file descriptors to fully detach */
            close(STDIN_FILENO);
            close(STDOUT_FILENO);
            close(STDERR_FILENO);
            
            /* Execute command */
            execl("/bin/sh", "/bin/sh", "-c", cmd, NULL);
            
            /* Should never reach here */
            _exit(EXIT_FAILURE);
        }
        /* Child exits immediately */
        _exit(EXIT_SUCCESS);
    }
    
    /* Parent waits for child to exit */
    wait(NULL);
}

/**
 * @brief Register a keybinding
 * 
 * @param mod Modifiers
 * @param keysym Key symbol
 * @param mode Key mode
 * @param func Function to call
 * @param arg Argument to pass to function
 * @param exec Command to execute
 * @return true if successful, false otherwise
 */
bool
register_keybinding(unsigned int mod, KeySym keysym, KeyMode mode, 
                   void (*func)(const Arg *), const Arg *arg, const char *exec)
{
    AdvancedKey *new_keys;
    
    /* Allocate or reallocate advanced keys array */
    new_keys = realloc(advanced_keys, (num_advanced_keys + 1) * sizeof(AdvancedKey));
    if (!new_keys)
        return false;
    
    advanced_keys = new_keys;
    
    /* Initialize new keybinding */
    advanced_keys[num_advanced_keys].mod = mod;
    advanced_keys[num_advanced_keys].keysym = keysym;
    advanced_keys[num_advanced_keys].mode = mode;
    advanced_keys[num_advanced_keys].func = func;
    
    /* Copy argument if provided */
    if (arg != NULL) {
        advanced_keys[num_advanced_keys].arg = *arg;
    } else {
        memset(&advanced_keys[num_advanced_keys].arg, 0, sizeof(Arg));
    }
    
    /* Copy exec command if provided */
    if (exec != NULL) {
        advanced_keys[num_advanced_keys].exec = strdup(exec);
        if (!advanced_keys[num_advanced_keys].exec) {
            fprintf(stderr, "DINA: Failed to allocate memory for keybinding command\n");
            return false;
        }
    } else {
        advanced_keys[num_advanced_keys].exec = NULL;
    }
    
    num_advanced_keys++;
    
    /* Regrab keys */
    grab_all_keys();
    
    return true;
}

/**
 * @brief Unregister a keybinding
 * 
 * @param mod Modifiers
 * @param keysym Key symbol
 * @param mode Key mode
 */
void
unregister_keybinding(unsigned int mod, KeySym keysym, KeyMode mode)
{
    unsigned int i, j;
    
    /* Find keybinding */
    for (i = 0; i < num_advanced_keys; i++) {
        if (advanced_keys[i].keysym == keysym
            && advanced_keys[i].mod == mod
            && advanced_keys[i].mode == mode) {
            
            /* Free exec command if allocated */
            if (advanced_keys[i].exec != NULL) {
                free(advanced_keys[i].exec);
            }
            
            /* Shift remaining keybindings */
            for (j = i; j < num_advanced_keys - 1; j++) {
                /* Copy entire structure */
                AdvancedKey temp = advanced_keys[j + 1];
                advanced_keys[j] = temp;
            }
            
            num_advanced_keys--;
            
            /* Reallocate array if not empty */
            if (num_advanced_keys > 0) {
                AdvancedKey *new_keys = realloc(advanced_keys, 
                                              num_advanced_keys * sizeof(AdvancedKey));
                if (new_keys != NULL)
                    advanced_keys = new_keys;
            } else {
                /* Free array if empty */
                free(advanced_keys);
                advanced_keys = NULL;
            }
            
            /* Regrab keys */
            grab_all_keys();
            
            return;
        }
    }
}

/**
 * @brief Register keybindings from config.h
 */
static void
register_config_keys(void)
{
    unsigned int i;
    
    /* First register standard keybindings in normal mode */
    for (i = 0; i < LENGTH(keys); i++) {
        if (keys[i].func) {
            register_keybinding(keys[i].mod, keys[i].keysym, MODE_NORMAL, 
                              keys[i].func, &keys[i].arg, NULL);
        }
    }
}

/**
 * @brief Start Orca screen reader directly
 */
void
start_orca(void)
{
    char *cmd = 
        "if ! pgrep -x 'orca' > /dev/null; then "
        "  play -nq synth 0.15 sine 500 sine 800 sine 1200 vol 0.4 & "
        "  sleep 0.5 && "
        "  orca --replace --no-setup & "
        "fi";
    
    execute_command(cmd);
}

/**
 * @brief Grab all registered keys
 */
void
grab_all_keys(void)
{
    unsigned int i, j;
    unsigned int modifiers[] = { 0, LockMask, numlockmask, numlockmask|LockMask };
    int start, end, skip;
    KeySym *syms;
    KeyCode keycode;
    
    /* Update numlock mask */
    updatenumlockmask();
    
    /* Ungrab all keys */
    XUngrabKey(dpy, AnyKey, AnyModifier, root);
    
    /* Get keyboard mapping */
    XDisplayKeycodes(dpy, &start, &end);
    syms = XGetKeyboardMapping(dpy, start, end - start + 1, &skip);
    if (!syms)
        return;
    
    /* Grab keys for the current mode only */
    for (i = 0; i < num_advanced_keys; i++) {
        /* Only grab keys for the current mode, plus Escape for all modes */
        if (advanced_keys[i].mode == current_mode || 
            (advanced_keys[i].keysym == XK_Escape && advanced_keys[i].mod == Mod4Mask)) {
            
            /* Convert keysym to keycode */
            keycode = XKeysymToKeycode(dpy, advanced_keys[i].keysym);
            if (keycode == NoSymbol)
                continue;
            
            /* Grab with all modifier combinations */
            for (j = 0; j < LENGTH(modifiers); j++) {
                XGrabKey(dpy, keycode,
                       advanced_keys[i].mod | modifiers[j],
                       root, True,
                       GrabModeAsync, GrabModeAsync);
            }
        }
    }
    
    /* If in normal mode, also grab original keybindings */
    if (current_mode == MODE_NORMAL) {
        for (i = 0; i < LENGTH(keys); i++) {
            /* Find keycode for the keysym */
            keycode = XKeysymToKeycode(dpy, keys[i].keysym);
            if (keycode == NoSymbol)
                continue;
            
            /* Grab with all modifier combinations */
            for (j = 0; j < LENGTH(modifiers); j++) {
                XGrabKey(dpy, keycode,
                       keys[i].mod | modifiers[j],
                       root, True,
                       GrabModeAsync, GrabModeAsync);
            }
        }
    }
    
    /* Free keyboard mapping */
    XFree(syms);
}