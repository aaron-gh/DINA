/**
 * @file event.c
 * @brief Event handling implementation for DINA
 *
 * This module handles X11 events in the DINA window manager.
 */

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

#include "event.h"
#include "dina.h"
#include "config.h"
#include "../wm/window.h"
#include "../wm/monitor.h"
#include "../wm/tag.h"
#include "../util/util.h"

/* Running state */
int running = 1;

/* Event handler function pointers */
static void (*handler[LASTEvent])(XEvent *) = {
    [ButtonPress] = buttonpress,
    [ClientMessage] = clientmessage,
    [ConfigureRequest] = configurerequest,
    [ConfigureNotify] = configurenotify,
    [DestroyNotify] = destroynotify,
    [EnterNotify] = enternotify,
    [Expose] = expose,
    [FocusIn] = focusin,
    [KeyPress] = keypress,
    [MappingNotify] = mappingnotify,
    [MapRequest] = maprequest,
    [MotionNotify] = motionnotify,
    [PropertyNotify] = propertynotify,
    [UnmapNotify] = unmapnotify
};

/**
 * @brief Initialize event handling
 * 
 * Set up event handlers and grab keys
 */
void
event_init(void)
{
    grabkeys();
}

/**
 * @brief Clean up event handling
 * 
 * Release resources used by event handling
 */
void
event_cleanup(void)
{
    /* Nothing to clean up at the moment */
}

/**
 * @brief Handle X events
 * 
 * Dispatch X events to appropriate handlers
 * 
 * @param e X event to handle
 */
void
handle_event(XEvent *e)
{
    /* Call handler */
    if (handler[e->type])
        handler[e->type](e);
}

/**
 * @brief Check for other window managers
 * 
 * Verify no other window manager is running
 */
void
checkotherwm(void)
{
    xerrorxlib = XSetErrorHandler(xerrorstart);
    /* This will trigger a BadAccess error if another WM is running */
    XSelectInput(dpy, DefaultRootWindow(dpy), SubstructureRedirectMask);
    XSync(dpy, False);
    XSetErrorHandler(xerror);
    XSync(dpy, False);
}

/**
 * @brief Grab keyboard keys
 * 
 * Set up key grabs for keyboard shortcuts
 */
void
grabkeys(void)
{
    updatenumlockmask();
    {
        unsigned int i, j;
        unsigned int modifiers[] = { 0, LockMask, numlockmask, numlockmask|LockMask };
        KeyCode code;

        XUngrabKey(dpy, AnyKey, AnyModifier, root);
        for (i = 0; i < LENGTH(keys); i++)
            if ((code = XKeysymToKeycode(dpy, keys[i].keysym)))
                for (j = 0; j < LENGTH(modifiers); j++)
                    XGrabKey(dpy, code, keys[i].mod | modifiers[j], root,
                        True, GrabModeAsync, GrabModeAsync);
    }
}

/**
 * @brief Update numlock mask
 * 
 * Get the current numlock mask
 */
void
updatenumlockmask(void)
{
    unsigned int i, j;
    XModifierKeymap *modmap;

    numlockmask = 0;
    modmap = XGetModifierMapping(dpy);
    for (i = 0; i < 8; i++)
        for (j = 0; j < modmap->max_keypermod; j++)
            if (modmap->modifiermap[i * modmap->max_keypermod + j]
                == XKeysymToKeycode(dpy, XK_Num_Lock))
                numlockmask = (1 << i);
    XFreeModifiermap(modmap);
}

/**
 * @brief Handle button press events
 * 
 * @param e X event
 */
void
buttonpress(XEvent *e)
{
    unsigned int i, x, click;
    Arg arg = {0};
    Client *c;
    Monitor *m;
    XButtonPressedEvent *ev = &e->xbutton;

    click = ClkRootWin;
    /* Focus monitor if necessary */
    if ((m = wintomon(ev->window)) && m != selmon) {
        unfocus(selmon->sel, 1);
        selmon = m;
        focus(NULL);
    }
    if (ev->window == selmon->barwin) {
        i = x = 0;
        do
            x += TEXTW(tags[i]);
        while (ev->x >= x && ++i < LENGTH(tags));
        if (i < LENGTH(tags)) {
            click = ClkTagBar;
            arg.ui = 1 << i;
        } else if (ev->x < selmon->ww) {
            click = ClkLtSymbol;
        } else
            click = ClkStatusText;
    } else if ((c = wintoclient(ev->window))) {
        focus(c);
        click = ClkClientWin;
    }
    for (i = 0; i < LENGTH(buttons); i++)
        if (click == buttons[i].click && buttons[i].func && buttons[i].button == ev->button
        && CLEANMASK(buttons[i].mask) == CLEANMASK(ev->state))
            buttons[i].func(click == ClkTagBar && buttons[i].arg.i == 0 ? &arg : &buttons[i].arg);
}

/**
 * @brief Handle client message events
 * 
 * @param e X event
 */
void
clientmessage(XEvent *e)
{
    XClientMessageEvent *cme = &e->xclient;
    Client *c = wintoclient(cme->window);

    if (!c)
        return;
    if (cme->message_type == netatom[NetWMState]) {
        if (cme->data.l[1] == netatom[NetWMFullscreen]
        || cme->data.l[2] == netatom[NetWMFullscreen])
            setfullscreen(c, (cme->data.l[0] == 1 /* _NET_WM_STATE_ADD    */
                || (cme->data.l[0] == 2 /* _NET_WM_STATE_TOGGLE */ && !c->isfullscreen)));
    } else if (cme->message_type == netatom[NetActiveWindow]) {
        if (c != selmon->sel && !c->isurgent)
            seturgent(c, 1);
    }
}

/* Other event handler implementations will be added here */

/**
 * @brief X error handler
 * 
 * @param dpy Display
 * @param ee X error event
 * @return Error code
 */
int
xerror(Display *dpy, XErrorEvent *ee)
{
    if (ee->error_code == BadWindow
    || (ee->request_code == X_SetInputFocus && ee->error_code == BadMatch)
    || (ee->request_code == X_PolyText8 && ee->error_code == BadDrawable)
    || (ee->request_code == X_PolyFillRectangle && ee->error_code == BadDrawable)
    || (ee->request_code == X_PolySegment && ee->error_code == BadDrawable)
    || (ee->request_code == X_ConfigureWindow && ee->error_code == BadMatch)
    || (ee->request_code == X_GrabButton && ee->error_code == BadAccess)
    || (ee->request_code == X_GrabKey && ee->error_code == BadAccess)
    || (ee->request_code == X_CopyArea && ee->error_code == BadDrawable))
        return 0;
    fprintf(stderr, "DINA: fatal error: request code=%d, error code=%d\n",
        ee->request_code, ee->error_code);
    return xerrorxlib(dpy, ee); /* may call exit */
}

/**
 * @brief Dummy X error handler
 * 
 * @param dpy Display
 * @param ee X error event
 * @return Always 0
 */
int
xerrordummy(Display *dpy, XErrorEvent *ee)
{
    return 0;
}

/**
 * @brief Startup X error handler
 * 
 * @param dpy Display
 * @param ee X error event
 * @return Error code
 */
int
xerrorstart(Display *dpy, XErrorEvent *ee)
{
    die("DINA: another window manager is already running");
    return -1;
}