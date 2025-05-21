/**
 * @file monitor.h
 * @brief Monitor management functionality for DINA
 *
 * This file contains the functions for managing monitors in the DINA window manager.
 */

#ifndef _MONITOR_H
#define _MONITOR_H

#include "../core/dina.h"

/**
 * @brief Create a new monitor
 * 
 * @return Newly created monitor
 */
Monitor *createmon(void);

/**
 * @brief Clean up a monitor
 * 
 * Free resources associated with a monitor
 * 
 * @param mon Monitor to clean up
 */
void cleanupmon(Monitor *mon);

/**
 * @brief Arrange windows on a monitor
 * 
 * Apply the current layout to a monitor's windows
 * 
 * @param m Monitor to arrange
 */
void arrange(Monitor *m);

/**
 * @brief Find a monitor from coordinates
 * 
 * @param x X coordinate
 * @param y Y coordinate
 * @param w Width
 * @param h Height
 * @return Monitor at the given coordinates
 */
Monitor *recttomon(int x, int y, int w, int h);

/**
 * @brief Find monitor from a window
 * 
 * @param w Window to find monitor for
 * @return Monitor containing the window
 */
Monitor *wintomon(Window w);

/**
 * @brief Update monitor geometry based on X configuration
 * 
 * @return 1 if changes were made, 0 otherwise
 */
int updategeom(void);

/**
 * @brief Update bar position
 * 
 * @param m Monitor to update bar position for
 */
void updatebarpos(Monitor *m);

/**
 * @brief Move focus to a different monitor
 * 
 * @param arg Command argument (.i = direction)
 */
void focusmon(const Arg *arg);

/**
 * @brief Apply the monocle layout to a monitor
 * 
 * Arrange windows in monocle (fullscreen) mode
 * 
 * @param m Monitor to apply layout to
 */
void monocle(Monitor *m);

/* Removed tile layout - only using monocle */

/**
 * @brief Get monitor in given direction
 * 
 * @param dir Direction (-1 for previous, +1 for next)
 * @return Monitor in the specified direction
 */
Monitor *dirtomon(int dir);

/**
 * @brief Send client to monitor
 * 
 * @param c Client to send
 * @param m Target monitor
 */
void sendmon(Client *c, Monitor *m);

#endif /* _MONITOR_H */