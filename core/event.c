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
#include "../wm/rules.h"
#include "../ui/ui.h"
#include "../util/util.h"

/* Running state is defined in dina.c */
extern int running;
extern Display *dpy;
extern Window root;
extern int sw, sh, bh;
extern Monitor *selmon, *mons;
extern Drw *drw;
extern char stext[256];
extern Atom wmatom[WMLast], netatom[NetLast];
extern unsigned int numlockmask;

/* From config.c */
extern const char *tags[];
extern const Key keys[];
extern const Button buttons[];

/* From window.c */
extern void focus(Client *c);
extern void unfocus(Client *c, int setfocus);
extern void setfocus(Client *c);
extern void seturgent(Client *c, int urg);
extern void updatewmhints(Client *c);
extern void updatetitle(Client *c);
extern void updatewindowtype(Client *c);
extern void setclientstate(Client *c, long state);
extern void unmanage(Client *c, int destroyed);
extern Client *wintoclient(Window w);
extern void manage(Window w, XWindowAttributes *wa);
extern void setfullscreen(Client *c, int fullscreen);
extern void resizeclient(Client *c, int x, int y, int w, int h);
extern void restack(Monitor *m);
extern void configure(Client *c);

/* From monitor.c */
extern int updategeom(void);
extern Monitor *wintomon(Window w);
extern Monitor *recttomon(int x, int y, int w, int h);
extern void arrange(Monitor *m);

/* From ui.c */
extern void drawbar(Monitor *m);
extern void drawbars(void);
extern void updatebars(void);
extern void updatestatus(void);
extern void updateclientlist(void);

/* Error handler function pointer */
static int (*xerrorxlib)(Display *, XErrorEvent *);

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
    /* Keys are now handled by the advanced keybinding system */
    /* grabkeys() is called from keys_init() */
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
 * @brief Run the event loop
 * 
 * Process X events until termination
 */
/* This function was moved to dina.c */

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
    XSetErrorHandler(xerrorxlib);
    XSync(dpy, False);
}

/* Functions moved to config.c */

/**
 * @brief Handle button press events
 * 
 * @param e X event
 */
void
buttonpress(XEvent *e)
{
    unsigned int i, click;
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
    /* Only handle client window clicks - simplified for headless mode */
    if ((c = wintoclient(ev->window))) {
        focus(c);
        restack(selmon);
        XAllowEvents(dpy, ReplayPointer, CurrentTime);
        click = ClkClientWin;
    }
    /* Find matching button and execute function */
    for (i = 0; i < LENGTH(buttons); i++)
        if (click == buttons[i].click && buttons[i].func && buttons[i].button == ev->button
        && CLEANMASK(buttons[i].mask) == CLEANMASK(ev->state))
            buttons[i].func(&buttons[i].arg);
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
        if ((Atom)cme->data.l[1] == netatom[NetWMFullscreen]
        || (Atom)cme->data.l[2] == netatom[NetWMFullscreen])
            setfullscreen(c, (cme->data.l[0] == 1 /* _NET_WM_STATE_ADD    */
                || (cme->data.l[0] == 2 /* _NET_WM_STATE_TOGGLE */ && !c->isfullscreen)));
    } else if (cme->message_type == netatom[NetActiveWindow]) {
        if (c != selmon->sel && !c->isurgent)
            seturgent(c, 1);
    }
}

/**
 * @brief Handle configure request events
 * 
 * @param e X event
 */
void
configurerequest(XEvent *e)
{
    Client *c;
    Monitor *m;
    XConfigureRequestEvent *ev = &e->xconfigurerequest;
    XWindowChanges wc;

    if ((c = wintoclient(ev->window))) {
        if (ev->value_mask & CWBorderWidth)
            c->bw = ev->border_width;
        else if (c->isfloating || !selmon->lt[selmon->sellt]->arrange) {
            m = c->mon;
            if (ev->value_mask & CWX) {
                c->oldx = c->x;
                c->x = m->mx + ev->x;
            }
            if (ev->value_mask & CWY) {
                c->oldy = c->y;
                c->y = m->my + ev->y;
            }
            if (ev->value_mask & CWWidth) {
                c->oldw = c->w;
                c->w = ev->width;
            }
            if (ev->value_mask & CWHeight) {
                c->oldh = c->h;
                c->h = ev->height;
            }
            if ((c->x + c->w) > m->mx + m->mw && c->isfloating)
                c->x = m->mx + (m->mw / 2 - WIDTH(c) / 2); /* center in x direction */
            if ((c->y + c->h) > m->my + m->mh && c->isfloating)
                c->y = m->my + (m->mh / 2 - HEIGHT(c) / 2); /* center in y direction */
            if ((ev->value_mask & (CWX|CWY)) && !(ev->value_mask & (CWWidth|CWHeight)))
                configure(c);
            if (ISVISIBLE(c))
                XMoveResizeWindow(dpy, c->win, c->x, c->y, c->w, c->h);
        } else
            configure(c);
    } else {
        wc.x = ev->x;
        wc.y = ev->y;
        wc.width = ev->width;
        wc.height = ev->height;
        wc.border_width = ev->border_width;
        wc.sibling = ev->above;
        wc.stack_mode = ev->detail;
        XConfigureWindow(dpy, ev->window, ev->value_mask, &wc);
    }
    XSync(dpy, False);
}

/**
 * @brief Handle configure notify events
 * 
 * @param e X event
 */
void
configurenotify(XEvent *e)
{
    Monitor *m;
    Client *c;
    XConfigureEvent *ev = &e->xconfigure;
    int dirty;

    /* updategeom handling sucks, needs to be simplified */
    if (ev->window == root) {
        dirty = (sw != ev->width || sh != ev->height);
        sw = ev->width;
        sh = ev->height;
        if (updategeom() || dirty) {
            drw_resize(drw, sw, bh);
            updatebars();
            for (m = mons; m; m = m->next) {
                for (c = m->clients; c; c = c->next)
                    if (c->isfullscreen)
                        resizeclient(c, m->mx, m->my, m->mw, m->mh);
                XMoveResizeWindow(dpy, m->barwin, m->wx, m->by, m->ww, bh);
            }
            focus(NULL);
            arrange(NULL);
        }
    }
}

/**
 * @brief Handle destroy notify events
 * 
 * @param e X event
 */
void
destroynotify(XEvent *e)
{
    Client *c;
    XDestroyWindowEvent *ev = &e->xdestroywindow;

    if ((c = wintoclient(ev->window)))
        unmanage(c, 1);
}

/**
 * @brief Handle enter notify events
 * 
 * @param e X event
 */
void
enternotify(XEvent *e)
{
    Client *c;
    Monitor *m;
    XCrossingEvent *ev = &e->xcrossing;

    if ((ev->mode != NotifyNormal || ev->detail == NotifyInferior) && ev->window != root)
        return;
    c = wintoclient(ev->window);
    m = c ? c->mon : wintomon(ev->window);
    if (m != selmon) {
        unfocus(selmon->sel, 1);
        selmon = m;
    } else if (!c || c == selmon->sel)
        return;
    focus(c);
}

/**
 * @brief Handle expose events
 * 
 * @param e X event
 */
void
expose(XEvent *e)
{
    Monitor *m;
    XExposeEvent *ev = &e->xexpose;

    if (ev->count == 0 && (m = wintomon(ev->window)))
        drawbar(m);
}

/**
 * @brief Handle focus in events
 * 
 * @param e X event
 */
void
focusin(XEvent *e)
{
    XFocusChangeEvent *ev = &e->xfocus;

    if (selmon->sel && ev->window != selmon->sel->win)
        setfocus(selmon->sel);
}

/**
 * @brief Handle key press events
 * 
 * @param e X event
 */
void
keypress(XEvent *e)
{
    /* Use the advanced keybinding system */
    handle_keypress(e);
}

/**
 * @brief Handle mapping notify events
 * 
 * @param e X event
 */
void
mappingnotify(XEvent *e)
{
    XMappingEvent *ev = &e->xmapping;

    XRefreshKeyboardMapping(ev);
    if (ev->request == MappingKeyboard) {
        /* Use the advanced keybinding system instead of grabkeys() */
        extern void grab_all_keys(void);
        grab_all_keys();
    }
}

/**
 * @brief Handle map request events
 * 
 * @param e X event
 */
void
maprequest(XEvent *e)
{
    static XWindowAttributes wa;
    XMapRequestEvent *ev = &e->xmaprequest;

    if (!XGetWindowAttributes(dpy, ev->window, &wa) || wa.override_redirect)
        return;
    if (!wintoclient(ev->window))
        manage(ev->window, &wa);
}

/**
 * @brief Handle motion notify events
 * 
 * @param e X event
 */
void
motionnotify(XEvent *e)
{
    static Monitor *mon = NULL;
    Monitor *m;
    XMotionEvent *ev = &e->xmotion;

    if (ev->window != root)
        return;
    if ((m = recttomon(ev->x_root, ev->y_root, 1, 1)) != mon && mon) {
        unfocus(selmon->sel, 1);
        selmon = m;
        focus(NULL);
    }
    mon = m;
}

/**
 * @brief Handle property notify events
 * 
 * @param e X event
 */
void
propertynotify(XEvent *e)
{
    Client *c;
    Window trans;
    XPropertyEvent *ev = &e->xproperty;

    if ((ev->window == root) && (ev->atom == XA_WM_NAME))
        updatestatus();
    else if (ev->state == PropertyDelete)
        return; /* ignore */
    else if ((c = wintoclient(ev->window))) {
        switch(ev->atom) {
        default: break;
        case XA_WM_TRANSIENT_FOR:
            if (!c->isfloating && (XGetTransientForHint(dpy, c->win, &trans)) &&
                (c->isfloating = (wintoclient(trans)) != NULL))
                arrange(c->mon);
            break;
        case XA_WM_NORMAL_HINTS:
            c->hintsvalid = 0;
            break;
        case XA_WM_HINTS:
            updatewmhints(c);
            drawbars();
            break;
        }
        if (ev->atom == XA_WM_NAME || ev->atom == netatom[NetWMName]) {
            updatetitle(c);
            if (c == c->mon->sel)
                drawbar(c->mon);
        }
        if (ev->atom == netatom[NetWMWindowType])
            updatewindowtype(c);
    }
}

/**
 * @brief Handle unmap notify events
 * 
 * @param e X event
 */
void
unmapnotify(XEvent *e)
{
    Client *c;
    XUnmapEvent *ev = &e->xunmap;

    if ((c = wintoclient(ev->window))) {
        if (ev->send_event)
            setclientstate(c, WithdrawnState);
        else
            unmanage(c, 0);
    }
}

/**
 * @brief Send an event to a window
 * 
 * @param c Client to send event to
 * @param proto Atom representing the protocol
 * @return 1 if successful, 0 otherwise
 */
/* sendevent moved to window.c */

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