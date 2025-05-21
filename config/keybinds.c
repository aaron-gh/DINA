/**
 * @file keybinds.c
 * @brief Keybinding configuration for DINA
 *
 * Implementation of runtime keybinding configuration
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <X11/keysym.h>
#include <X11/X.h>

#include "config.h"
#include "../core/dina.h"

/* Function declarations */
static void parse_keybind_action(DINAKeybind *kb);
static bool handle_internal_action(const char *action, const char *arg);
static void execute_command(const char *cmd);

/**
 * @brief Parse a keybinding string from config
 * 
 * Format: "modifier+key" or "modifier+key+shift"
 * 
 * @param kb_str Keybinding string
 * @param kb Keybinding to populate
 * @return true if parsed successfully, false otherwise
 */
bool
parse_keybind(const char *kb_str, DINAKeybind *kb)
{
    char copy[128];
    char *part, *saveptr;
    int part_num = 0;
    
    if (!kb_str || !kb)
        return false;
    
    /* Initialize keybinding */
    kb->modifier = 0;
    kb->additional_mod = 0;
    kb->keysym = 0;
    
    /* Make a copy for strtok operations */
    strncpy(copy, kb_str, sizeof(copy) - 1);
    copy[sizeof(copy) - 1] = '\0';
    
    /* Parse parts */
    part = strtok_r(copy, "+", &saveptr);
    while (part) {
        part_num++;
        
        if (part_num == 1) {
            /* First part should be 'modifier' */
            if (strcmp(part, "modifier") == 0) {
                /* This will be replaced with actual modifier later */
                kb->modifier = 0;
            } else {
                /* Unexpected format */
                return false;
            }
        } else if (part_num == 2) {
            /* Second part should be key */
            kb->keysym = config_parse_key(part);
            if (kb->keysym == 0)
                return false;
        } else if (part_num == 3) {
            /* Third part should be additional modifier */
            kb->additional_mod = config_parse_mod(part);
            if (kb->additional_mod == 0)
                return false;
        } else {
            /* Too many parts */
            return false;
        }
        
        part = strtok_r(NULL, "+", &saveptr);
    }
    
    /* Validate */
    if (part_num < 2)
        return false;
    
    return true;
}

/**
 * @brief Handle a keybinding
 * 
 * @param kb Keybinding to handle
 */
void
handle_keybind(DINAKeybind *kb)
{
    if (!kb)
        return;
    
    /* Parse action */
    parse_keybind_action(kb);
}

/**
 * @brief Parse and handle keybinding action
 * 
 * @param kb Keybinding with action
 */
static void
parse_keybind_action(DINAKeybind *kb)
{
    char action[64];
    char arg[128] = "";
    char *colon;
    
    /* Copy action for parsing */
    strncpy(action, kb->action, sizeof(action) - 1);
    action[sizeof(action) - 1] = '\0';
    
    /* Check if action has an argument (format: action:arg) */
    colon = strchr(action, ':');
    if (colon) {
        /* Split into action and argument */
        *colon = '\0';
        strncpy(arg, colon + 1, sizeof(arg) - 1);
        arg[sizeof(arg) - 1] = '\0';
    }
    
    /* Handle internal action or external command */
    if (strcmp(action, "exec") == 0) {
        /* Execute external command */
        execute_command(arg);
    } else {
        /* Try to handle internal action */
        if (!handle_internal_action(action, arg)) {
            fprintf(stderr, "Unknown action: %s\n", action);
        }
    }
}

/**
 * @brief Handle internal DINA action
 * 
 * @param action Action name
 * @param arg Action argument
 * @return true if handled, false otherwise
 */
static bool
handle_internal_action(const char *action, const char *arg)
{
    /* View tag actions */
    if (strncmp(action, "view_tag_", 9) == 0) {
        int tag_num = atoi(action + 9);
        if (tag_num >= 1 && tag_num <= 9) {
            /* This will be implemented in a later phase */
            return true;
        }
    }
    
    /* Move to tag actions */
    if (strncmp(action, "move_to_tag_", 12) == 0) {
        int tag_num = atoi(action + 12);
        if (tag_num >= 1 && tag_num <= 9) {
            /* This will be implemented in a later phase */
            return true;
        }
    }
    
    /* Navigation actions */
    if (strcmp(action, "focus_next") == 0) {
        /* This will be implemented in a later phase */
        return true;
    } else if (strcmp(action, "focus_previous") == 0) {
        /* This will be implemented in a later phase */
        return true;
    } else if (strcmp(action, "close_window") == 0) {
        /* This will be implemented in a later phase */
        return true;
    } else if (strcmp(action, "quit") == 0) {
        /* This will be implemented in a later phase */
        return true;
    }
    
    return false;
}

/**
 * @brief Execute external command
 * 
 * @param cmd Command to execute
 */
static void
execute_command(const char *cmd)
{
    if (!cmd || !*cmd)
        return;
    
    /* Fork and execute command */
    if (fork() == 0) {
        setsid();
        execl("/bin/sh", "/bin/sh", "-c", cmd, NULL);
        exit(EXIT_SUCCESS);
    }
}