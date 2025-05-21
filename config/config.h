/**
 * @file config.h
 * @brief Configuration system for DINA
 *
 * Header file for runtime configuration system
 */

#ifndef __DINA_CONFIG_H__
#define __DINA_CONFIG_H__

#include <stdbool.h>
#include <X11/keysym.h>
#include <X11/X.h>

/* Config file location */
#define CONFIG_PATH "/.config/dina/config"

/**
 * Keybinding structure
 */
typedef struct {
    unsigned int modifier;      /* Modifier key (e.g., Mod4Mask) */
    unsigned int additional_mod; /* Additional modifier (e.g., ShiftMask) or 0 if none */
    KeySym keysym;              /* Key symbol */
    char action[64];            /* Action to perform */
    char arg[128];              /* Argument for action (if any) */
} DINAKeybind;

/**
 * Autostart program structure
 */
typedef struct {
    char name[64];              /* Program name */
    char command[256];          /* Command to execute */
} DINAAutostart;

/**
 * Sound configuration structure
 */
typedef struct {
    bool enabled;               /* Whether sounds are enabled */
    char sound_dir[256];        /* Directory containing sound files */
    char startup_sound[128];    /* Sound file for startup */
    char workspace_sound[128];  /* Sound file for workspace change */
    char window_close_sound[128]; /* Sound file for window close */
    char window_focus_sound[128]; /* Sound file for window focus */
    char error_sound[128];      /* Sound file for errors */
} DINASoundConfig;

/**
 * Speech configuration structure
 */
typedef struct {
    bool enabled;               /* Whether speech is enabled */
    bool announce_window_changes; /* Whether to announce window changes */
    bool announce_workspace_changes; /* Whether to announce workspace changes */
    float rate;                 /* Speech rate */
    float pitch;                /* Speech pitch */
} DINASpeechConfig;

/**
 * Rule configuration structure
 */
typedef struct {
    char selector[128];         /* Window selector (class:instance:title) */
    unsigned int tags;          /* Tags to assign */
    bool floating;              /* Whether window should float */
    int monitor;                /* Monitor to place window on */
} DINARule;

/**
 * Main configuration structure
 */
typedef struct {
    /* Appearance */
    unsigned int border_width;  /* Border width in pixels */
    unsigned int snap_distance; /* Snap distance in pixels */
    bool show_bar;              /* Whether to show bar */
    char font[128];             /* Font for visible elements */
    
    /* Layout */
    bool resize_hints;          /* Whether to respect size hints */
    bool lock_fullscreen;       /* Whether to force focus on fullscreen windows */
    
    /* Workspaces */
    char tag_names[9][32];      /* Names for workspaces */
    
    /* Keybindings */
    DINAKeybind *keybinds;      /* Array of keybindings */
    int num_keybinds;           /* Number of keybindings */
    
    /* Autostart */
    DINAAutostart *autostart;   /* Array of autostart programs */
    int num_autostart;          /* Number of autostart programs */
    
    /* Accessibility */
    DINASoundConfig sound;      /* Sound configuration */
    DINASpeechConfig speech;    /* Speech configuration */
    
    /* Rules */
    DINARule *rules;            /* Array of rules */
    int num_rules;              /* Number of rules */
} DINAConfig;

/* Global configuration */
extern DINAConfig dina_config;

/* Function declarations */
void dina_config_init(void);
void dina_config_cleanup(void);
bool dina_config_load(void);
void dina_config_create_default(void);
KeySym config_parse_key(const char *key_str);
unsigned int config_parse_mod(const char *mod_str);
unsigned int config_parse_tag(const char *tag_str);
void config_run_autostart(void);
void dina_config_apply(void);

#endif /* __DINA_CONFIG_H__ */