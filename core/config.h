/**
 * @file config.h
 * @brief Configuration file for DINA
 */

#ifndef _CONFIG_H
#define _CONFIG_H

#include <X11/X.h>
#include <X11/keysym.h>
#include "dina.h"
#include "../wm/window.h"
#include "../wm/monitor.h"
#include "../wm/tag.h"

/* Forward declarations for functions from monitor.h, window.h, and tag.h */ 
extern void monocle(Monitor *m);
extern void focusstack(const Arg *arg);
extern void killclient(const Arg *arg);
extern void view(const Arg *arg);
extern void tag(const Arg *arg);
extern void movemouse(const Arg *arg);
extern void resizemouse(const Arg *arg);
extern void quit(const Arg *arg);

/* appearance settings */
static const unsigned int borderpx  = 1;           // Border pixel of windows
static const unsigned int snap      = 32;          // Snap pixel for window movement
static const int showbar            = 0;           // No status bar — we're headless
static const int topbar             = 1;           // Irrelevant since bar is hidden
__attribute__((unused)) static const char *fonts[] = { "monospace:size=10" }; // Font for any visible fallback
__attribute__((unused)) static const char dmenufont[] = "monospace:size=10"; // Placeholder, dmenu not used
static const char col_black[]       = "#000000";   // All visual elements black for screen reader use

/* For ui/drw.c */
enum { SchemeNorm, SchemeSel, SchemeLast }; // Color schemes

/* Click locations for mouse actions - simplified for headless mode */
enum { ClkClientWin, ClkRootWin, ClkLast }; /* Removed status bar related click locations */

// Color scheme (also all black)
__attribute__((unused)) static const char *colors[][3] = {
	[SchemeNorm] = { col_black, col_black, col_black }, // Normal: fg, bg, border
	[SchemeSel]  = { col_black, col_black, col_black }, // Selected window
};

/* tags (virtual workspaces) */
#define NUMTAGS 9  /* Number of tags */
extern const char *tags[]; /* Defined in config.c */

/* no window rules */
static const Rule rules[] = { 
	/* class      instance    title       tags mask     isfloating   monitor */
	{ NULL,       NULL,       NULL,       0,            0,           -1 }
};

/* layout: only monocle is kept */
static const float mfact     = 0.55;    // Master area size (unused in monocle)
static const int nmaster     = 1;       // Number of windows in master (unused in monocle)
static const int resizehints = 0;       // Ignore size hints
static const int lockfullscreen = 1;    // Force focus on fullscreen windows

static const Layout layouts[] = {
	{ "[M]",      monocle },             // Only layout available
};

/* key definitions */
#define MODKEY Mod4Mask                // Use Super/Windows key as modifier
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} },

/* helper macro to spawn shell commands */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* Removed dmenu references - not needed */

/* key bindings — minimal, accessible */
static const Key keys[] = {
	{ MODKEY,                       XK_j,      focusstack,     {.i = -1 } }, // Focus previous
	{ MODKEY,                       XK_l,      focusstack,     {.i = +1 } }, // Focus next
	{ MODKEY,                       XK_k,      killclient,     {0} },        // Close window
	/* Tag keys for switching workspaces and moving windows */
	{ MODKEY,                       XK_1,      view,           {.ui = 1 << 0} },
	{ MODKEY|ShiftMask,             XK_1,      tag,            {.ui = 1 << 0} },
	{ MODKEY,                       XK_2,      view,           {.ui = 1 << 1} },
	{ MODKEY|ShiftMask,             XK_2,      tag,            {.ui = 1 << 1} },
	{ MODKEY,                       XK_3,      view,           {.ui = 1 << 2} },
	{ MODKEY|ShiftMask,             XK_3,      tag,            {.ui = 1 << 2} },
	{ MODKEY,                       XK_4,      view,           {.ui = 1 << 3} },
	{ MODKEY|ShiftMask,             XK_4,      tag,            {.ui = 1 << 3} },
	{ MODKEY,                       XK_5,      view,           {.ui = 1 << 4} },
	{ MODKEY|ShiftMask,             XK_5,      tag,            {.ui = 1 << 4} },
	{ MODKEY,                       XK_6,      view,           {.ui = 1 << 5} },
	{ MODKEY|ShiftMask,             XK_6,      tag,            {.ui = 1 << 5} },
	{ MODKEY,                       XK_7,      view,           {.ui = 1 << 6} },
	{ MODKEY|ShiftMask,             XK_7,      tag,            {.ui = 1 << 6} },
	{ MODKEY,                       XK_8,      view,           {.ui = 1 << 7} },
	{ MODKEY|ShiftMask,             XK_8,      tag,            {.ui = 1 << 7} },
	{ MODKEY,                       XK_9,      view,           {.ui = 1 << 8} },
	{ MODKEY|ShiftMask,             XK_9,      tag,            {.ui = 1 << 8} },
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} }         // Quit DINA
};

/* mouse button definitions - minimal for headless mode */
static const Button buttons[] = {
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	/* Removed all status bar and tag bar buttons - not needed for headless mode */
};

/* Config functions */
void config_init(void);
void grabkeys(void);
void updatenumlockmask(void);
void grabbuttons(Client *c, int focused);
unsigned int cleanmask(unsigned int mask);
const Key *config_get_key(KeySym keysym, unsigned int mod);
const Button *config_get_button(unsigned int click, unsigned int button, unsigned int mod);

#endif /* _CONFIG_H */
