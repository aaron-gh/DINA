/**
 * @file notify.h
 * @brief Notification and audio feedback functionality for DINA
 *
 * This file contains the functions for providing audio and speech feedback
 * in the DINA window manager, crucial for blind users.
 */

#ifndef _NOTIFY_H
#define _NOTIFY_H

#include "core/dina.h"

/**
 * @brief Initialize notification subsystem
 * 
 * Set up required resources for notifications
 */
void notify_init(void);

/**
 * @brief Clean up notification subsystem
 * 
 * Free resources used by notifications
 */
void notify_cleanup(void);

/**
 * @brief Play startup notification
 * 
 * Announce that DINA has started
 */
void notify_startup(void);

/**
 * @brief Notify when a tag is changed
 * 
 * Play a sound and announce the new tag
 * 
 * @param tag Tag number (1-based)
 */
void notify_tag(int tag);

/**
 * @brief Notify when a window is moved between tags
 * 
 * Play a sound and announce the move
 * 
 * @param from_tag Source tag (1-based)
 * @param to_tag Destination tag (1-based)
 */
void notify_window_move(int from_tag, int to_tag);

/**
 * @brief Notify when an application is no longer tracked for workspace memory
 * 
 * Announce that an application is no longer being tracked
 * 
 * @param class Window class
 * @param instance Window instance
 */
void notify_window_untracked(const char *class, const char *instance);

/**
 * @brief Notify when an application is automatically placed on a tag
 * 
 * Announce automatic placement of an application
 * 
 * @param class Window class
 * @param instance Window instance
 * @param tag Tag number (1-based)
 */
void notify_tag_placement(const char *class, const char *instance, int tag);

/**
 * @brief Helper macro to spawn a notification command
 * 
 * @param cmd Shell command to execute for notification
 */
#define SHCMD_NOTIFY(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

#endif /* _NOTIFY_H */