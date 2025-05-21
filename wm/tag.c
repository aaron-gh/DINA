/**
 * @file tag.c
 * @brief Tag/workspace management implementation for DINA
 *
 * This module handles tag (workspace) functionality.
 */

#include <stdlib.h>
#include <X11/Xlib.h>

#include "../core/dina.h"
#include "../core/config.h"
#include "tag.h"
#include "window.h"
#include "monitor.h"
#include "../util/util.h"
#include "../a11y/notify.h"

/* External variables from dina.c */
extern Monitor *selmon;
extern const char *tags[];
extern void spawn(const Arg *arg);

/* Helper macro to spawn shell commands */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* Tag mask - all valid tags */
#define TAGMASK ((1 << 9) - 1)

/**
 * @brief View a tag
 * 
 * Switch to the specified tag (workspace)
 * 
 * @param arg Command argument (.ui = tag mask)
 */
void
view(const Arg *arg)
{
    if ((arg->ui & TAGMASK) == selmon->tagset[selmon->seltags])
        return;
    
    selmon->seltags ^= 1; /* toggle sel tagset */
    
    if (arg->ui & TAGMASK) {
        selmon->tagset[selmon->seltags] = arg->ui & TAGMASK;
        
        // Find the tag number (1-based) for notification
        int i, tag_num = 0;
        for (i = 0; i < NUMTAGS; i++) {
            if (arg->ui & (1 << i)) {
                tag_num = i + 1;
                break;
            }
        }
        
        // Notify about tag change
        if (tag_num > 0) {
            notify_tag(tag_num);
        }
    }
    
    focus(NULL);
    arrange(selmon);
}

/**
 * @brief Tag a window
 * 
 * Assign a tag to the selected window
 * 
 * @param arg Command argument (.ui = tag mask)
 */
void
tag(const Arg *arg)
{
    if (selmon->sel && arg->ui & TAGMASK) {
        // Find the previous tag (1-based)
        int old_tag = 0;
        int i;
        for (i = 0; i < NUMTAGS; i++) {
            if (selmon->sel->tags & (1 << i)) {
                old_tag = i + 1;
                break;
            }
        }
        
        // Set new tag
        selmon->sel->tags = arg->ui & TAGMASK;
        
        // Find the new tag (1-based)
        int new_tag = 0;
        for (i = 0; i < NUMTAGS; i++) {
            if (selmon->sel->tags & (1 << i)) {
                new_tag = i + 1;
                break;
            }
        }
        
        // Notify about tag change if both old and new tags are valid
        if (old_tag > 0 && new_tag > 0 && old_tag != new_tag) {
            notify_window_move(old_tag, new_tag);
        }
        
        focus(NULL);
        arrange(selmon);
    }
}

/* Removed toggleview and toggletag functions - not used in our simplified UI */

/**
 * @brief Move a window to a different monitor
 * 
 * @param arg Command argument (.i = monitor direction)
 */
void
tagmon(const Arg *arg)
{
    if (!selmon->sel || !mons->next)
        return;
    sendmon(selmon->sel, dirtomon(arg->i));
}

/**
 * @brief Notify when switching to a tag
 * 
 * Provide audio feedback when switching to a tag
 * 
 * @param tag Tag number (1-based)
 */
/* These notification functions are in a11y/notify.c */

/**
 * @brief Get next tiled client
 * 
 * @param c Starting client
 * @return Next tiled client or NULL
 */
Client *
nexttiled(Client *c)
{
    for (; c && (c->isfloating || !ISVISIBLE(c)); c = c->next);
    return c;
}