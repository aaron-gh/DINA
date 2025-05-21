/**
 * @file ui.h
 * @brief User interface functions for DINA
 *
 * This file contains functions for managing the DINA user interface.
 */

#ifndef _UI_H
#define _UI_H

#include "../core/dina.h"

/**
 * @brief Create status bars
 */
void updatebars(void);

/**
 * @brief Draw status bar for a monitor
 * 
 * @param m Monitor to draw bar for
 */
void drawbar(Monitor *m);

/**
 * @brief Draw all status bars
 */
void drawbars(void);

/**
 * @brief Update status text
 */
void updatestatus(void);

/**
 * @brief Update client list
 * 
 * Update list of managed clients
 */
void updateclientlist(void);

#endif /* _UI_H */