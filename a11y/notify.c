/**
 * @file notify.c
 * @brief Audio notification implementation for DINA
 *
 * This module handles audio notifications and feedback.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../core/dina.h"
#include "notify.h"
#include "../util/util.h"

/* External variables from dina.c */
extern const char *broken;

/* Forward declaration for spawning */
static void spawn(const Arg *arg);

/**
 * @brief Spawn a process
 * 
 * Helper function to execute notification commands
 * 
 * @param arg Command argument
 */
static void
spawn(const Arg *arg)
{
    if (fork() == 0) {
        if (fork() == 0) {
            /* Grandchild process to prevent zombies */
            const char **cmd = (const char **)arg->v;
            setsid();
            execvp(cmd[0], (char **)cmd);
            fprintf(stderr, "DINA: execvp %s", cmd[0]);
            perror(" failed");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS); /* Child exits immediately */
    }
    /* Parent continues */
}

/**
 * @brief Initialize notifications
 * 
 * Set up audio notification system
 */
void
notify_init(void)
{
    /* Check if speech-dispatcher is available */
    Arg arg = { .v = (const char*[]){ "which", "spd-say", NULL } };
    spawn(&arg);
    
    /* Check if SoX play command is available */
    arg.v = (const char*[]){ "which", "play", NULL };
    spawn(&arg);
}

/**
 * @brief Clean up notifications
 * 
 * Release resources used by notifications
 */
void
notify_cleanup(void)
{
    /* Nothing to clean up - resource mgmt handled by OS */
}

/**
 * @brief Notify on startup
 * 
 * Play sound on DINA startup
 */
void
notify_startup(void)
{
    /* Play a distinctive startup sound and announce that DINA has started */
    char cmd[256];
    snprintf(cmd, sizeof(cmd), 
        "play -nq synth 0.15 sine 500 sine 800 sine 1200 vol 0.4 & spd-say -r -30 \"DINA window manager started\"");
    
    Arg arg = SHCMD_NOTIFY(cmd);
    spawn(&arg);
}

/**
 * @brief Notify when tag is changed
 * 
 * Play a sound and announce the new tag
 * 
 * @param tag Tag number (1-based)
 */
void
notify_tag(int tag)
{
    /* Play a unique sound for tag switching */
    char cmd[256];
    snprintf(cmd, sizeof(cmd), 
        "play -nq synth 0.1 sine %d vol 0.3 & spd-say -r -50 \"Tag %d\"", 
        400 + (tag * 50), tag);
    
    Arg arg = SHCMD_NOTIFY(cmd);
    spawn(&arg);
}

/**
 * @brief Notify when a window is moved between tags
 * 
 * Play a sound and announce the move
 * 
 * @param from_tag Source tag (1-based)
 * @param to_tag Destination tag (1-based)
 */
void
notify_window_move(int from_tag, int to_tag)
{
    /* Play a sound for window movement between tags and announce it */
    char cmd[256];
    snprintf(cmd, sizeof(cmd), 
        "play -nq synth 0.1 sine %d sine %d vol 0.3 & spd-say -r -40 \"Window moved from tag %d to tag %d\"", 
        400 + (from_tag * 50), 400 + (to_tag * 50), from_tag, to_tag);
    
    Arg arg = SHCMD_NOTIFY(cmd);
    spawn(&arg);
}

/**
 * @brief Notify when window is untracked
 * 
 * @param class Window class
 * @param instance Window instance
 */
void
notify_window_untracked(const char *class, const char *instance)
{
    char cmd[1024];
    char app_name[256];
    
    /* Use instance as app name, or class if instance is "broken" */
    if (strcmp(instance, broken) == 0)
        strncpy(app_name, class, sizeof(app_name) - 1);
    else
        strncpy(app_name, instance, sizeof(app_name) - 1);
    
    app_name[sizeof(app_name) - 1] = '\0'; /* Ensure null-termination */
    
    /* Create notification command */
    snprintf(cmd, sizeof(cmd), 
        "play -nq synth 0.1 sine 600 sine 400 vol 0.3 & spd-say -r -40 \"%s no longer tracked\"", 
        app_name);
    
    Arg arg = SHCMD_NOTIFY(cmd);
    spawn(&arg);
}

/**
 * @brief Notify window placement on tag
 * 
 * @param class Window class
 * @param instance Window instance
 * @param tag Tag number
 */
void
notify_tag_placement(const char *class, const char *instance, int tag)
{
    if (tag <= 1)  /* Don't notify for tag 1 */
        return;
    
    char cmd[1024];
    char app_name[256];
    
    /* Use instance as app name, or class if instance is "broken" */
    if (strcmp(instance, broken) == 0)
        strncpy(app_name, class, sizeof(app_name) - 1);
    else
        strncpy(app_name, instance, sizeof(app_name) - 1);
    
    app_name[sizeof(app_name) - 1] = '\0'; /* Ensure null-termination */
    
    /* Create notification command */
    snprintf(cmd, sizeof(cmd), 
        "play -nq synth 0.1 sine %d sine %d vol 0.3 & spd-say -r -40 \"%s automatically placed on tag %d\"", 
        400 + (tag * 50), 500 + (tag * 50), app_name, tag);
    
    Arg arg = SHCMD_NOTIFY(cmd);
    spawn(&arg);
}