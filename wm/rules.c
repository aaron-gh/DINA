/**
 * @file rules.c
 * @brief Window rule implementation for DINA
 *
 * This module handles applying rules to windows based on properties.
 */

#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include "../core/dina.h"
#include "../core/config.h"
#include "rules.h"
#include "window.h"
#include "../util/util.h"
#include "../a11y/workspace_memory.h"

/* External variables from dina.c */
extern Display *dpy;
extern Monitor *selmon;
extern Monitor *mons;
extern const char *broken;
extern const Rule rules[];
extern Atom wmatom[WMLast], netatom[NetLast];

/* External functions from window.c */
extern int gettextprop(Window w, Atom atom, char *text, unsigned int size);

/**
 * @brief Apply rules to a client
 * 
 * Check window properties and apply matching rules
 * 
 * @param c Client to apply rules to
 */
void
applyrules(Client *c)
{
    const char *class, *instance;
    unsigned int i;
    const Rule *r;
    Monitor *m;
    XClassHint ch = { NULL, NULL };
    int from_persistence = 0;

    /* Default values */
    c->isfloating = 0;
    c->tags = 0;

    /* Get window class and instance */
    XGetClassHint(dpy, c->win, &ch);
    class = ch.res_class ? ch.res_class : broken;
    instance = ch.res_name ? ch.res_name : broken;

    /* First check for persistent tag assignments from workspace memory */
    from_persistence = applypersistworkspace(c, class, instance);

    /* If no persistent assignment or it returned 0, apply standard rules */
    if (!from_persistence || c->tags == 0) {
        for (i = 0; i < LENGTH(rules); i++) {
            r = &rules[i];
            if ((!r->title || strstr(c->name, r->title))
                && (!r->class || strstr(class, r->class))
                && (!r->instance || strstr(instance, r->instance)))
            {
                c->isfloating = r->isfloating;
                c->tags |= r->tags;
                for (m = mons; m && m->num != r->monitor; m = m->next);
                if (m)
                    c->mon = m;
            }
        }
    }

    /* If still no tags assigned, use current tags */
    if (!c->tags)
        c->tags = selmon->tagset[selmon->seltags];

    /* Clean up */
    if (ch.res_class)
        XFree(ch.res_class);
    if (ch.res_name)
        XFree(ch.res_name);
}