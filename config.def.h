/* appearance settings */
static const unsigned int borderpx  = 1;           // Border pixel of windows
static const unsigned int snap      = 32;          // Snap pixel for window movement
static const int showbar            = 0;           // No status bar — we're headless
static const int topbar             = 1;           // Irrelevant since bar is hidden
static const char *fonts[]          = { "monospace:size=10" }; // Font for any visible fallback
static const char dmenufont[]       = "monospace:size=10";     // Placeholder, dmenu not used
static const char col_black[]       = "#000000";   // All visual elements black for screen reader use

// Color scheme (also all black)
static const char *colors[][3]      = {
	[SchemeNorm] = { col_black, col_black, col_black }, // Normal: fg, bg, border
	[SchemeSel]  = { col_black, col_black, col_black }, // Selected window
};

/* tags (virtual workspaces) */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

/* no window rules */
static const Rule rules[] = { 
	/* class      instance    title       tags mask     isfloating   monitor */
	{ NULL,       NULL,       NULL,       0,            0,           -1 }
};

/* layout: only monocle is kept */
static const float mfact     = 0.55;    // Master area size (unused in monocle)
static const int nmaster     = 1;       // Number of windows in master (unused in monocle)
static const int resizehints = 1;       // Respect size hints
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

/* dummy dmenu command to satisfy references — not used */
static char dmenumon[2] = "0";
static const char *dmenucmd[] = { "true", NULL };

/* key bindings — minimal, accessible */
static const Key keys[] = {
	{ MODKEY,                       XK_j,      focusstack,     {.i = -1 } }, // Focus previous
	{ MODKEY,                       XK_l,      focusstack,     {.i = +1 } }, // Focus next
	{ MODKEY,                       XK_k,      killclient,     {0} },        // Close window
	/* Focus window by index (F1-F12) */
	{ MODKEY,                       XK_F1,     focuswindowbyindex, {.i = 0} },
	{ MODKEY,                       XK_F2,     focuswindowbyindex, {.i = 1} },
	{ MODKEY,                       XK_F3,     focuswindowbyindex, {.i = 2} },
	{ MODKEY,                       XK_F4,     focuswindowbyindex, {.i = 3} },
	{ MODKEY,                       XK_F5,     focuswindowbyindex, {.i = 4} },
	{ MODKEY,                       XK_F6,     focuswindowbyindex, {.i = 5} },
	{ MODKEY,                       XK_F7,     focuswindowbyindex, {.i = 6} },
	{ MODKEY,                       XK_F8,     focuswindowbyindex, {.i = 7} },
	{ MODKEY,                       XK_F9,     focuswindowbyindex, {.i = 8} },
	{ MODKEY,                       XK_F10,    focuswindowbyindex, {.i = 9} },
	{ MODKEY,                       XK_F11,    focuswindowbyindex, {.i = 10} },
	{ MODKEY,                       XK_F12,    focuswindowbyindex, {.i = 11} },
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

/* mouse button definitions — mostly left as placeholders */
static const Button buttons[] = {
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        NULL,           {0} }, // disabled togglefloating
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        NULL,           {0} }, // toggleview removed
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        NULL,           {0} }, // toggletag removed
};
