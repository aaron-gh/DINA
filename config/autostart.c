/**
 * @file autostart.c
 * @brief Autostart configuration for DINA
 *
 * Implementation of runtime autostart configuration
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "config.h"
#include "../core/dina.h"

/**
 * @brief Parse autostart programs from config
 * 
 * @param section_text Text of autostart section
 * @return true if parsed successfully, false otherwise
 */
bool
parse_autostart(const char *section_text)
{
    /* This will be implemented in a later phase */
    return true;
}

/**
 * @brief Expand ~ to home directory
 */
static char*
expand_home_path(const char *path)
{
    if (path[0] == '~' && path[1] == '/') {
        char *home = getenv("HOME");
        if (home) {
            char *result = malloc(strlen(home) + strlen(path));
            if (result) {
                strcpy(result, home);
                strcat(result, path + 1);
                return result;
            }
        }
    }
    
    return strdup(path);
}

/**
 * @brief Run autostart programs
 */
void
run_autostart(void)
{
    int i;
    char *cmd;
    
    /* Execute each autostart command */
    for (i = 0; i < dina_config.num_autostart; i++) {
        cmd = expand_home_path(dina_config.autostart[i].command);
        if (cmd) {
            fprintf(stderr, "Autostarting: %s\n", cmd);
            if (fork() == 0) {
                setsid();
                execl("/bin/sh", "/bin/sh", "-c", cmd, NULL);
                exit(EXIT_SUCCESS);
            }
            free(cmd);
        }
    }
}

/**
 * @brief Add autostart program
 * 
 * @param name Program name
 * @param command Command to execute
 * @return true if added successfully, false otherwise
 */
bool
add_autostart(const char *name, const char *command)
{
    DINAAutostart *new_autostart;
    
    /* Resize autostart array */
    new_autostart = realloc(dina_config.autostart, 
                          (dina_config.num_autostart + 1) * sizeof(DINAAutostart));
    if (!new_autostart)
        return false;
    
    dina_config.autostart = new_autostart;
    
    /* Add new program */
    strncpy(dina_config.autostart[dina_config.num_autostart].name, name, 
            sizeof(dina_config.autostart[dina_config.num_autostart].name) - 1);
    strncpy(dina_config.autostart[dina_config.num_autostart].command, command,
            sizeof(dina_config.autostart[dina_config.num_autostart].command) - 1);
    
    dina_config.num_autostart++;
    
    return true;
}

/**
 * @brief Remove autostart program
 * 
 * @param name Program name
 * @return true if removed successfully, false otherwise
 */
bool
remove_autostart(const char *name)
{
    int i, j;
    
    /* Find program */
    for (i = 0; i < dina_config.num_autostart; i++) {
        if (strcmp(dina_config.autostart[i].name, name) == 0) {
            /* Remove by shifting array */
            for (j = i; j < dina_config.num_autostart - 1; j++) {
                strcpy(dina_config.autostart[j].name, dina_config.autostart[j + 1].name);
                strcpy(dina_config.autostart[j].command, dina_config.autostart[j + 1].command);
            }
            
            dina_config.num_autostart--;
            
            /* Resize array */
            if (dina_config.num_autostart > 0) {
                DINAAutostart *new_autostart = realloc(dina_config.autostart,
                                  dina_config.num_autostart * sizeof(DINAAutostart));
                if (new_autostart)
                    dina_config.autostart = new_autostart;
            } else {
                free(dina_config.autostart);
                dina_config.autostart = NULL;
            }
            
            return true;
        }
    }
    
    return false;
}