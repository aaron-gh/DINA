/**
 * @file window.h
 * @brief Window management functionality for DINA
 *
 * This file contains the functions for managing windows in the DINA window manager.
 */

#ifndef _WINDOW_H
#define _WINDOW_H

#include "core/dina.h"

/* Window Management Functions */

/**
 * @brief Update client window title
 * 
 * @param c Client to update
 */
void updatetitle(Client *c);

/**
 * @brief Apply rules to a new window
 * 
 * @param c Client to apply rules to
 */
void applyrules(Client *c);

/**
 * @brief Manage a new window
 * 
 * @param w X Window to manage
 * @param wa X Window attributes
 */
void manage(Window w, XWindowAttributes *wa);

/**
 * @brief Stop managing a client
 * 
 * @param c Client to unmanage
 * @param destroyed Whether the window has been destroyed
 */
void unmanage(Client *c, int destroyed);

/**
 * @brief Resize a client
 * 
 * @param c Client to resize
 * @param x New x position
 * @param y New y position
 * @param w New width
 * @param h New height
 * @param interact Whether this is an interactive resize
 */
void resize(Client *c, int x, int y, int w, int h, int interact);

/**
 * @brief Resize client window
 * 
 * @param c Client to resize
 * @param x New x position
 * @param y New y position
 * @param w New width
 * @param h New height
 */
void resizeclient(Client *c, int x, int y, int w, int h);

/**
 * @brief Apply size hints
 * 
 * @param c Client to apply size hints to
 * @param x Pointer to x position
 * @param y Pointer to y position
 * @param w Pointer to width
 * @param h Pointer to height
 * @param interact Whether this is an interactive resize
 * @return int 1 if size hints applied, 0 otherwise
 */
int applysizehints(Client *c, int *x, int *y, int *w, int *h, int interact);

/**
 * @brief Focus a client
 * 
 * @param c Client to focus (NULL for default behavior)
 */
void focus(Client *c);

/**
 * @brief Set focus to a client
 * 
 * @param c Client to set focus to
 */
void setfocus(Client *c);

/**
 * @brief Unfocus a client
 * 
 * @param c Client to unfocus
 * @param setfocus Whether to set focus elsewhere
 */
void unfocus(Client *c, int setfocus);

/**
 * @brief Attach a client to the client list
 * 
 * @param c Client to attach
 */
void attach(Client *c);

/**
 * @brief Detach a client from the client list
 * 
 * @param c Client to detach
 */
void detach(Client *c);

/**
 * @brief Set client's fullscreen state
 * 
 * @param c Client to set fullscreen state for
 * @param fullscreen 1 for fullscreen, 0 for normal
 */
void setfullscreen(Client *c, int fullscreen);

/**
 * @brief Close a client window
 * 
 * @param arg Command argument (unused)
 */
void killclient(const Arg *arg);

/**
 * @brief Move focus to next/previous client
 * 
 * @param arg Command argument (.i = direction)
 */
void focusstack(const Arg *arg);

#endif /* _WINDOW_H */