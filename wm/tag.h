/**
 * @file tag.h
 * @brief Tag (workspace) management functionality for DINA
 *
 * This file contains the functions for managing tags/workspaces in the DINA window manager.
 */

#ifndef _TAG_H
#define _TAG_H

#include "../core/dina.h"

/**
 * @brief View a tag
 * 
 * Switch to the specified tag (workspace)
 * 
 * @param arg Command argument (.ui = tag mask)
 */
void view(const Arg *arg);

/**
 * @brief Tag a window
 * 
 * Assign a tag to the selected window
 * 
 * @param arg Command argument (.ui = tag mask)
 */
void tag(const Arg *arg);

/**
 * @brief Toggle a tag in view
 * 
 * Add or remove a tag from the current view
 * 
 * @param arg Command argument (.ui = tag mask)
 */
void toggleview(const Arg *arg);

/**
 * @brief Toggle a tag on a window
 * 
 * Add or remove a tag from the selected window
 * 
 * @param arg Command argument (.ui = tag mask)
 */
void toggletag(const Arg *arg);

/**
 * @brief Move a window to a different monitor
 * 
 * @param arg Command argument (.i = monitor direction)
 */
void tagmon(const Arg *arg);

/**
 * @brief Notify when switching to a tag
 * 
 * Provide audio feedback when switching to a tag
 * 
 * @param tag Tag number (1-based)
 */
void notify_tag(int tag);

/**
 * @brief Notify when moving a window between tags
 * 
 * Provide audio feedback when moving a window between tags
 * 
 * @param from_tag Source tag (1-based)
 * @param to_tag Destination tag (1-based)
 */
void notify_window_move(int from_tag, int to_tag);

/**
 * @brief Get next tiled client
 * 
 * @param c Starting client
 * @return Next tiled client or NULL
 */
Client *nexttiled(Client *c);

#endif /* _TAG_H */