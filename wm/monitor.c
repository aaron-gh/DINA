/**
 * @file monitor.c
 * @brief Monitor management implementation for DINA
 *
 * This module handles monitor detection, configuration, and management.
 */

#include <stdlib.h>
#include <X11/Xlib.h>

#include "../core/dina.h"
#include "monitor.h"
#include "window.h"
#include "tag.h"
#include "../util/util.h"

/* External variables from dina.c */
extern Display *dpy;
extern Window root;
extern int sw, sh, bh;
extern Monitor *mons;
extern Monitor *selmon;

/* Macros */
#define INTERSECT(x,y,w,h,m) (MAX(0, MIN((x)+(w),(m)->wx+(m)->ww) - MAX((x),(m)->wx)) \
                          * MAX(0, MIN((y)+(h),(m)->wy+(m)->wh) - MAX((y),(m)->wy)))
/* ISVISIBLE is defined in util.h */
#define WIDTH(X)  ((X)->w + 2 * (X)->bw)

/* Forward declarations */
static void arrangemon(Monitor *m);
static int getrootptr(int *x, int *y);
static void showhide(Client *c);

/**
 * @brief Create a new monitor
 * 
 * @return Newly created monitor
 */
Monitor *
createmon(void)
{
    Monitor *m = ecalloc(1, sizeof(Monitor));
    return m;
}

/**
 * @brief Clean up a monitor
 * 
 * Free resources associated with a monitor
 * 
 * @param mon Monitor to clean up
 */
void
cleanupmon(Monitor *mon)
{
    Monitor *m;
    
    if (mon->barwin)
        XDestroyWindow(dpy, mon->barwin);
    for (m = mons; m && m->next != mon; m = m->next);
    if (m)
        m->next = mon->next;
    else
        mons = mon->next;
    free(mon);
}

/**
 * @brief Arrange windows on a monitor
 * 
 * Apply the current layout to a monitor's windows
 * 
 * @param m Monitor to arrange
 */
void
arrange(Monitor *m)
{
    if (m)
        showhide(m->stack);
    else 
        for (m = mons; m; m = m->next)
            showhide(m->stack);
    
    if (m) {
        arrangemon(m);
        restack(m);
    } else 
        for (m = mons; m; m = m->next)
            arrangemon(m);
}

/**
 * @brief Find a monitor from coordinates
 * 
 * @param x X coordinate
 * @param y Y coordinate
 * @param w Width
 * @param h Height
 * @return Monitor at the given coordinates
 */
Monitor *
recttomon(int x, int y, int w, int h)
{
    Monitor *m, *r = selmon;
    int a, area = 0;

    for (m = mons; m; m = m->next)
        if ((a = INTERSECT(x, y, w, h, m)) > area) {
            area = a;
            r = m;
        }
    return r;
}

/**
 * @brief Find monitor from a window
 * 
 * @param w Window to find monitor for
 * @return Monitor containing the window
 */
Monitor *
wintomon(Window w)
{
    int x, y;
    Client *c;
    Monitor *m;

    if (w == root && getrootptr(&x, &y))
        return recttomon(x, y, 1, 1);
    for (m = mons; m; m = m->next)
        if (w == m->barwin)
            return m;
    if ((c = wintoclient(w)))
        return c->mon;
    return selmon;
}

/**
 * @brief Update monitor geometry based on X configuration
 * Simplified implementation for single monitor setup
 * 
 * @return 1 if changes were made, 0 otherwise
 */
int
updategeom(void)
{
    /* Create default monitor if none exist */
    if (!mons) {
        mons = createmon();
        mons->num = 0;
    }
    
    /* Use full screen size for the monitor */
    for (Monitor *m = mons; m; m = m->next) {
        m->mx = m->wx = 0;
        m->my = m->wy = 0;
        m->mw = m->ww = sw;
        m->mh = m->wh = sh;
        updatebarpos(m);
    }
    
    return 1;
}

/**
 * @brief Update bar position (simplified for headless mode)
 * 
 * @param m Monitor to update bar position for
 */
void
updatebarpos(Monitor *m)
{
    m->wy = m->my;
    m->wh = m->mh;
    m->by = -bh; /* Bar is always hidden */
}

/**
 * @brief Get monitor in a specific direction
 * 
 * @param dir Direction (-1 for previous, +1 for next)
 * @return Monitor in the specified direction
 */
Monitor *
dirtomon(int dir)
{
    Monitor *m = NULL;

    if (dir > 0) {
        if (!(m = selmon->next))
            m = mons;
    } else if (selmon == mons)
        for (m = mons; m->next; m = m->next);
    else
        for (m = mons; m->next != selmon; m = m->next);
    return m;
}

/**
 * @brief Move focus to a different monitor
 * 
 * @param arg Command argument (.i = direction)
 */
void
focusmon(const Arg *arg)
{
    Monitor *m;

    if (!mons->next)
        return;
    if ((m = dirtomon(arg->i)) == selmon)
        return;
    unfocus(selmon->sel, 0);
    selmon = m;
    focus(NULL);
}

/**
 * @brief Apply the monocle layout to a monitor
 * 
 * Arrange windows in monocle (fullscreen) mode
 * 
 * @param m Monitor to apply layout to
 */
void
monocle(Monitor *m)
{
    Client *c;
    
    for (c = nexttiled(m->clients); c; c = nexttiled(c->next))
        resize(c, m->wx, m->wy, m->ww - 2 * c->bw, m->wh - 2 * c->bw, 0);
}

/* We only use monocle layout, tile is removed */

/**
 * @brief Send a client to a different monitor
 * 
 * @param c Client to send
 * @param m Target monitor
 */
void
sendmon(Client *c, Monitor *m)
{
    if (!c || !m || c->mon == m)
        return;
    unfocus(c, 1);
    detach(c);
    detachstack(c);
    c->mon = m;
    c->tags = m->tagset[m->seltags]; /* Assign current tags of target monitor */
    attach(c);
    attachstack(c);
    focus(NULL);
    arrange(NULL);
}

/**
 * @brief Arrange windows on a monitor
 * 
 * Apply current layout to monitor
 * 
 * @param m Monitor to arrange
 */
static void
arrangemon(Monitor *m)
{
    strncpy(m->ltsymbol, m->lt[m->sellt]->symbol, sizeof m->ltsymbol);
    if (m->lt[m->sellt]->arrange)
        m->lt[m->sellt]->arrange(m);
}

/**
 * @brief Get pointer coordinates on root window
 * 
 * @param x Pointer to store X coordinate
 * @param y Pointer to store Y coordinate
 * @return 1 if successful, 0 otherwise
 */
static int
getrootptr(int *x, int *y)
{
    int di;
    unsigned int dui;
    Window dummy;

    return XQueryPointer(dpy, root, &dummy, &dummy, x, y, &di, &di, &dui);
}

/**
 * @brief Show or hide a client window
 * 
 * @param c Client to show or hide
 */
static void
showhide(Client *c)
{
    if (!c)
        return;
    if (ISVISIBLE(c)) {
        /* show clients top down */
        XMoveWindow(dpy, c->win, c->x, c->y);
        if ((!c->mon->lt[c->mon->sellt]->arrange || c->isfloating) && !c->isfullscreen)
            resize(c, c->x, c->y, c->w, c->h, 0);
        showhide(c->snext);
    } else {
        /* hide clients bottom up */
        showhide(c->snext);
        XMoveWindow(dpy, c->win, WIDTH(c) * -2, c->y);
    }
}