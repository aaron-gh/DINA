/**
 * @file notify.c
 * @brief Notification and audio feedback implementation
 *
 * This module provides audio and speech feedback for DINA,
 * which is crucial for blind users.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "notify.h"
#include "../core/dina.h"
#include "../util/util.h"

/**
 * @brief Initialize notification subsystem
 * 
 * Set up required resources for notifications
 */
void
notify_init(void)
{
    /* Nothing to initialize at the moment */
}

/**
 * @brief Clean up notification subsystem
 * 
 * Free resources used by notifications
 */
void
notify_cleanup(void)
{
    /* Nothing to clean up at the moment */
}

/**
 * @brief Play startup notification
 * 
 * Announce that DINA has started
 */
void
notify_startup(void)
{
    /* Play a distinctive startup sound and announce that DINA has started */
    /* This helps users know DINA is running even if other components (like Orca) fail */
    char cmd[512];
    
    snprintf(cmd, sizeof(cmd), 
        "play -nq synth 0.15 sine 500 sine 800 sine 1200 vol 0.4 & spd-say -r -30 \"DINA window manager started\"");
    
    Arg arg = SHCMD_NOTIFY(cmd);
    spawn(&arg);
}

/**
 * @brief Notify when a tag is changed
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
    char cmd[512];
    
    snprintf(cmd, sizeof(cmd), 
        "play -nq synth 0.1 sine %d sine %d vol 0.3 & spd-say -r -40 \"Window moved from tag %d to tag %d\"", 
        400 + (from_tag * 50), 400 + (to_tag * 50), from_tag, to_tag);
    
    Arg arg = SHCMD_NOTIFY(cmd);
    spawn(&arg);
}

/**
 * @brief Notify when an application is no longer tracked for workspace memory
 * 
 * Announce that an application is no longer being tracked
 * 
 * @param class Window class
 * @param instance Window instance
 */
void
notify_window_untracked(const char *class, const char *instance)
{
    char cmd[512];
    char app_name[256];
    
    /* Use instance as app name, or class if instance is "broken" */
    if (strcmp(instance, broken) == 0)
        strncpy(app_name, class, sizeof(app_name) - 1);
    else
        strncpy(app_name, instance, sizeof(app_name) - 1);
    
    /* Create notification command */
    snprintf(cmd, sizeof(cmd), 
        "play -nq synth 0.1 sine 600 sine 400 vol 0.3 & spd-say -r -40 \"%s no longer tracked\"", 
        app_name);
    
    Arg arg = SHCMD_NOTIFY(cmd);
    spawn(&arg);
}

/**
 * @brief Notify when an application is automatically placed on a tag
 * 
 * Announce automatic placement of an application
 * 
 * @param class Window class
 * @param instance Window instance
 * @param tag Tag number (1-based)
 */
void
notify_tag_placement(const char *class, const char *instance, int tag)
{
    if (tag <= 1)  /* Don't notify for tag 1 */
        return;
    
    char cmd[512];
    char app_name[256];
    
    /* Use instance as app name, or class if instance is "broken" */
    if (strcmp(instance, broken) == 0)
        strncpy(app_name, class, sizeof(app_name) - 1);
    else
        strncpy(app_name, instance, sizeof(app_name) - 1);
    
    /* Create notification command */
    snprintf(cmd, sizeof(cmd), 
        "play -nq synth 0.1 sine %d sine %d vol 0.3 & spd-say -r -40 \"%s automatically placed on tag %d\"", 
        400 + (tag * 50), 500 + (tag * 50), app_name, tag);
    
    Arg arg = SHCMD_NOTIFY(cmd);
    spawn(&arg);
}