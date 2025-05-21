/**
 * @file dina.c
 * @brief Main entry point for DINA (DINA Is Not Aesthetics)
 *
 * DINA is a stripped-down, accessibility-first fork of dwm
 * tailored for blind users and screen reader workflows.
 */

#include <errno.h>
#include <locale.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xproto.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/Xft/Xft.h>

#include "dina.h"
#include "event.h"
#include "config.h"
#include "../wm/window.h"
#include "../wm/monitor.h"
#include "../wm/tag.h"
#include "../a11y/notify.h"
#include "../a11y/workspace_memory.h"
#include "../util/util.h"

/* Global variables */
Display *dpy;
Window root;
int screen;
int sw, sh;              /* X display screen geometry width, height */
int bh;                  /* bar height */
int lrpad;               /* sum of left and right padding for text */
unsigned int numlockmask = 0;
Drw *drw;
Monitor *mons, *selmon;  /* Monitor linked list and selected monitor */
const char *broken = "broken";
char stext[256];
int (*xerrorxlib)(Display *, XErrorEvent *);

/* Function prototypes */
static void sigchld(int unused);

/* External references to functions in other modules */
int running = 1;  /* global running state */
void scan(void);  /* scan for clients, defined in window.c */

/* EWMH atoms */
enum { NetSupported, NetWMName, NetWMState, NetWMCheck,
       NetWMFullscreen, NetActiveWindow, NetWMWindowType,
       NetWMWindowTypeDialog, NetClientList, NetLast }; /* EWMH atoms */
enum { WMProtocols, WMDelete, WMState, WMTakeFocus, WMLast }; /* default atoms */
Atom wmatom[WMLast], netatom[NetLast];
Window wmcheckwin;

/**
 * @brief Print version info
 */
static void
print_version(void)
{
    fprintf(stderr, "DINA-"VERSION"\n");
}

/**
 * @brief SIGCHLD handler
 * 
 * Clean up child processes
 * 
 * @param unused Signal number (unused)
 */
static void
sigchld(int unused)
{
    /* We don't care about the exit status */
    if (signal(SIGCHLD, sigchld) == SIG_ERR)
        die("Failed to reinstall SIGCHLD handler");
    while (0 < waitpid(-1, NULL, WNOHANG));
}

/**
 * @brief Main setup function
 * 
 * Initialize resources and set up DINA
 */
void
setup(void)
{
    XSetWindowAttributes wa;
    Atom utf8string;
    struct sigaction sa;

    /* Load configuration */
    config_init();
    
    /* Set up signal handlers */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_NOCLDSTOP | SA_RESTART | SA_NOCLDWAIT;
    sa.sa_handler = sigchld;
    sigaction(SIGCHLD, &sa, NULL);

    /* Initialize X resources */
    screen = DefaultScreen(dpy);
    sw = DisplayWidth(dpy, screen);
    sh = DisplayHeight(dpy, screen);
    root = RootWindow(dpy, screen);
    drw = drw_create(dpy, screen, root, sw, sh);
    
    /* Set up drawing context */
    if (!drw_fontset_create(drw, fonts, LENGTH(fonts)))
        die("No fonts could be loaded");
    lrpad = drw->fonts->h;
    bh = drw->fonts->h + 2;
    updategeom();

    /* Initialize atoms */
    utf8string = XInternAtom(dpy, "UTF8_STRING", False);
    wmatom[WMProtocols] = XInternAtom(dpy, "WM_PROTOCOLS", False);
    wmatom[WMDelete] = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
    wmatom[WMState] = XInternAtom(dpy, "WM_STATE", False);
    wmatom[WMTakeFocus] = XInternAtom(dpy, "WM_TAKE_FOCUS", False);
    netatom[NetActiveWindow] = XInternAtom(dpy, "_NET_ACTIVE_WINDOW", False);
    netatom[NetSupported] = XInternAtom(dpy, "_NET_SUPPORTED", False);
    netatom[NetWMName] = XInternAtom(dpy, "_NET_WM_NAME", False);
    netatom[NetWMState] = XInternAtom(dpy, "_NET_WM_STATE", False);
    netatom[NetWMCheck] = XInternAtom(dpy, "_NET_SUPPORTING_WM_CHECK", False);
    netatom[NetWMFullscreen] = XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False);
    netatom[NetWMWindowType] = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE", False);
    netatom[NetWMWindowTypeDialog] = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_DIALOG", False);
    netatom[NetClientList] = XInternAtom(dpy, "_NET_CLIENT_LIST", False);

    /* Set up EWMH support */
    wmcheckwin = XCreateSimpleWindow(dpy, root, 0, 0, 1, 1, 0, 0, 0);
    XChangeProperty(dpy, wmcheckwin, netatom[NetWMCheck], XA_WINDOW, 32,
        PropModeReplace, (unsigned char *) &wmcheckwin, 1);
    XChangeProperty(dpy, wmcheckwin, netatom[NetWMName], utf8string, 8,
        PropModeReplace, (unsigned char *) "DINA", 4);
    XChangeProperty(dpy, root, netatom[NetWMCheck], XA_WINDOW, 32,
        PropModeReplace, (unsigned char *) &wmcheckwin, 1);
    XChangeProperty(dpy, root, netatom[NetSupported], XA_ATOM, 32,
        PropModeReplace, (unsigned char *) netatom, NetLast);
    XDeleteProperty(dpy, root, netatom[NetClientList]);

    /* Select events */
    wa.cursor = 0; /* no cursor */
    wa.event_mask = SubstructureRedirectMask|SubstructureNotifyMask
        |ButtonPressMask|PointerMotionMask|EnterWindowMask
        |LeaveWindowMask|StructureNotifyMask|PropertyChangeMask;
    XChangeWindowAttributes(dpy, root, CWEventMask|CWCursor, &wa);
    XSelectInput(dpy, root, wa.event_mask);

    /* Initialize subsystems */
    grabkeys();
    event_init();
    notify_init();
    workspace_memory_init();

    /* Set initial focus */
    focus(NULL);

    /* Announce startup */
    notify_startup();
}

/**
 * @brief DINA main loop
 * 
 * Process events
 */
void
run(void)
{
    XEvent ev;
    /* Main event loop */
    while (running && !XNextEvent(dpy, &ev))
        handle_event(&ev);
}

/**
 * @brief Clean up DINA resources
 * 
 * Free resources before exit
 */
void
cleanup(void)
{
    Monitor *m;

    /* Clean up subsystems */
    event_cleanup();
    notify_cleanup();
    workspace_memory_cleanup();

    /* Free X resources */
    XUngrabKey(dpy, AnyKey, AnyModifier, root);
    while (mons) {
        m = mons;
        mons = mons->next;
        cleanupmon(m);
    }
    drw_free(drw);
    XSync(dpy, False);
    XSetInputFocus(dpy, PointerRoot, RevertToPointerRoot, CurrentTime);
    XDeleteProperty(dpy, root, netatom[NetActiveWindow]);
}

/**
 * @brief DINA entry point
 * 
 * @param argc Argument count
 * @param argv Argument array
 * @return Exit code
 */
int
main(int argc, char *argv[])
{
    if (argc == 2 && !strcmp("-v", argv[1])) {
        print_version();
        return 0;
    } else if (argc != 1) {
        fprintf(stderr, "usage: dina [-v]\n");
        return 1;
    }

    /* Set locale */
    setlocale(LC_CTYPE, "");

    /* Open X display */
    if (!(dpy = XOpenDisplay(NULL)))
        die("DINA: cannot open display");

    /* Set up X error handler */
    checkotherwm();
    xerrorxlib = XSetErrorHandler(xerror);
    XSetErrorHandler(xerrorstart);
    
    /* Initialize and run DINA */
    setup();
    scan();
    run();

    /* Clean up and exit */
    cleanup();
    XCloseDisplay(dpy);
    return 0;
}