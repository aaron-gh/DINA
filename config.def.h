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
static const Rule rules[] = { 0 };

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
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },        // Quit DINA
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
