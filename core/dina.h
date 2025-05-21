/**
 * @file dina.h
 * @brief Main header file for DINA (DINA Is Not Aesthetics)
 *
 * This file contains the core structures and declarations for the DINA window manager,
 * a fork of dwm designed specifically for blind users and screen reader workflows.
 */

#ifndef _DINA_H
#define _DINA_H

#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
#include <X11/Xatom.h>

/* Includes from original dwm */
#include "ui/drw.h"

/* EWMH atoms */
enum { WMProtocols, WMDelete, WMState, WMTakeFocus, WMLast }; /* default atoms */
enum { NetSupported, NetWMName, NetWMState, NetWMCheck,
       NetWMFullscreen, NetActiveWindow, NetWMWindowType,
       NetWMWindowTypeDialog, NetClientList, NetLast }; /* EWMH atoms */

/* forward declarations */
typedef struct Client Client;
typedef struct Monitor Monitor;

/**
 * @brief Argument types for window manager commands
 *
 * Union to pass various argument types to window manager functions.
 */
typedef union {
    int i;             /**< Integer argument */
    unsigned int ui;   /**< Unsigned integer argument */
    float f;           /**< Float argument */
    const void *v;     /**< Pointer argument */
} Arg;

/**
 * @brief Button definition
 * 
 * Defines a mouse button action
 */
typedef struct {
    unsigned int click;  /**< Button click type */
    unsigned int mask;   /**< Modifier mask */
    unsigned int button; /**< Button number */
    void (*func)(const Arg *arg); /**< Function to call */
    const Arg arg;       /**< Argument to pass to function */
} Button;

/**
 * @brief Key definition
 * 
 * Defines a keyboard shortcut
 */
typedef struct {
    unsigned int mod;       /**< Modifier mask */
    KeySym keysym;          /**< X11 key symbol */
    void (*func)(const Arg *arg); /**< Function to call */
    const Arg arg;          /**< Argument to pass to function */
} Key;

/**
 * @brief Layout definition
 * 
 * Defines a layout for arranging windows
 */
typedef struct {
    const char *symbol; /**< Layout symbol */
    void (*arrange)(Monitor *); /**< Arrangement function */
} Layout;

/**
 * @brief Window rule
 * 
 * Rules for handling specific windows
 */
typedef struct {
    const char *class;   /**< Window class */
    const char *instance; /**< Window instance */
    const char *title;    /**< Window title */
    unsigned int tags;    /**< Tag mask */
    int isfloating;       /**< Floating status */
    int monitor;          /**< Monitor index */
} Rule;

/**
 * @brief Client structure
 * 
 * Represents a managed window
 */
struct Client {
    char name[256];       /**< Window name */
    float mina, maxa;     /**< Aspect ratios */
    int x, y, w, h;       /**< Window geometry */
    int oldx, oldy, oldw, oldh; /**< Previous geometry */
    int basew, baseh, incw, inch, maxw, maxh, minw, minh; /**< Size hints */
    int bw, oldbw;        /**< Border width */
    unsigned int tags;    /**< Tags (workspaces) */
    int isfixed, isfloating, isurgent, neverfocus, oldstate, isfullscreen; /**< States */
    int hintsvalid;       /**< Whether size hints are valid */
    Client *next;         /**< Next client in list */
    Client *snext;        /**< Next client in stack */
    Monitor *mon;         /**< Monitor */
    Window win;           /**< X window */
};

/**
 * @brief Monitor structure
 * 
 * Represents a physical monitor
 */
struct Monitor {
    char ltsymbol[16];   /**< Layout symbol */
    float mfact;         /**< Master area factor */
    int nmaster;         /**< Number of windows in master area */
    int num;             /**< Monitor number */
    int by;              /**< Bar geometry - Y position */
    int mx, my, mw, mh;  /**< Monitor geometry */
    int wx, wy, ww, wh;  /**< Window area geometry */
    unsigned int seltags; /**< Selected tag set */
    unsigned int sellt;  /**< Selected layout */
    unsigned int tagset[2]; /**< Tag sets (current and previous) */
    int showbar;         /**< Bar visibility */
    int topbar;          /**< Bar position (top or bottom) */
    Client *clients;     /**< List of clients */
    Client *sel;         /**< Selected client */
    Client *stack;       /**< Stack of clients */
    Monitor *next;       /**< Next monitor */
    Window barwin;       /**< Bar window */
    const Layout *lt[2]; /**< Current and previous layouts */
};

/* Function declarations */
extern void cleanup(void);
extern void setup(void);
extern void run(void);
extern int main(int argc, char *argv[]);

/* Global variable declarations (extern) */
extern Display *dpy;
extern Window root;
extern int screen;
extern int sw, sh;
extern int lrpad;              /* sum of left and right padding for text */
extern char stext[256];        /* status text */
extern unsigned int numlockmask;
extern Drw *drw;
extern Monitor *mons, *selmon;
extern const char *tags[];
extern Atom wmatom[WMLast], netatom[NetLast];

#endif /* _DINA_H */