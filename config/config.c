/**
 * @file config.c
 * @brief Configuration system for DINA
 *
 * Implementation of runtime configuration system
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <X11/keysym.h>

#include "config.h"
#include "../core/dina.h"
#include "../core/keys.h"
#include "../util/util.h"

/* Global configuration */
DINAConfig dina_config;

/* Default values for configuration */
static void config_set_defaults(void);
static bool parse_config_line(char *line, char *section);
static bool parse_key_value(const char *line, char *key, char *value);
static bool parse_section_header(const char *line, char *section);
static void trim(char *str);
static void config_parse_keybindings(void);
static void config_parse_appearance(void);
static void config_parse_layout(void);
static void config_parse_workspaces(void);
static void config_parse_autostart(void);
static void config_parse_accessibility(void);
static void config_parse_rules(void);
static bool is_section_start(const char *line);

/**
 * @brief Initialize configuration system
 */
void
dina_config_init(void)
{
    /* Set default configuration values */
    config_set_defaults();
    
    /* Load configuration file if available */
    if (!dina_config_load()) {
        /* Create default configuration file */
        dina_config_create_default();
    }
    
    /* Apply configuration */
    dina_config_apply();
}

/**
 * @brief Cleanup configuration system
 */
void
dina_config_cleanup(void)
{
    /* Free allocated memory */
    if (dina_config.keybinds) {
        free(dina_config.keybinds);
        dina_config.keybinds = NULL;
    }
    
    if (dina_config.autostart) {
        free(dina_config.autostart);
        dina_config.autostart = NULL;
    }
    
    if (dina_config.rules) {
        free(dina_config.rules);
        dina_config.rules = NULL;
    }
}

/**
 * @brief Set default configuration values
 */
static void
config_set_defaults(void)
{
    int i;
    
    /* Appearance */
    dina_config.border_width = 1;
    dina_config.snap_distance = 32;
    dina_config.show_bar = false;
    strncpy(dina_config.font, "monospace:size=10", sizeof(dina_config.font));
    
    /* Layout */
    dina_config.resize_hints = false;
    dina_config.lock_fullscreen = true;
    
    /* Workspaces */
    for (i = 0; i < 9; i++) {
        snprintf(dina_config.tag_names[i], sizeof(dina_config.tag_names[i]), "%d", i + 1);
    }
    
    /* Keybindings - will be populated later */
    dina_config.keybinds = NULL;
    dina_config.num_keybinds = 0;
    
    /* Autostart - will be populated later */
    dina_config.autostart = NULL;
    dina_config.num_autostart = 0;
    
    /* Accessibility - Sound */
    dina_config.sound.enabled = true;
    strcpy(dina_config.sound.sound_dir, "~/.local/share/dina/sounds/");
    strcpy(dina_config.sound.startup_sound, "startup.wav");
    strcpy(dina_config.sound.workspace_sound, "workspace.wav");
    strcpy(dina_config.sound.window_close_sound, "close.wav");
    strcpy(dina_config.sound.window_focus_sound, "focus.wav");
    strcpy(dina_config.sound.error_sound, "error.wav");
    
    /* Accessibility - Speech */
    dina_config.speech.enabled = true;
    dina_config.speech.announce_window_changes = true;
    dina_config.speech.announce_workspace_changes = true;
    dina_config.speech.rate = 1.0;
    dina_config.speech.pitch = 0.0;
    
    /* Rules - will be populated later */
    dina_config.rules = NULL;
    dina_config.num_rules = 0;
}

/**
 * @brief Expand ~ to home directory
 */
static char*
expand_home(const char *path)
{
    if (path[0] == '~' && path[1] == '/') {
        char *home = getenv("HOME");
        if (home) {
            char *result = malloc(strlen(home) + strlen(path));
            if (result) {
                strcpy(result, home);
                strcat(result, path + 1);
                return result;
            }
        }
    }
    
    return strdup(path);
}

/**
 * @brief Load configuration from file
 * 
 * @return true if config loaded successfully, false otherwise
 */
bool
dina_config_load(void)
{
    FILE *f;
    char path[512];
    char line[1024];
    char section[64] = "";
    char *home;
    
    /* Get path to config file */
    home = getenv("HOME");
    if (!home)
        return false;  /* Can't get home directory */
    
    snprintf(path, sizeof(path), "%s%s", home, CONFIG_PATH);
    
    /* Try to open the file */
    f = fopen(path, "r");
    if (!f)
        return false;  /* No config file yet */
    
    /* Read config file line by line */
    while (fgets(line, sizeof(line), f)) {
        /* Parse line */
        if (!parse_config_line(line, section)) {
            fprintf(stderr, "Error parsing config line: %s", line);
        }
    }
    
    fclose(f);
    
    /* Post-processing */
    config_parse_keybindings();
    config_parse_appearance();
    config_parse_layout();
    config_parse_workspaces();
    config_parse_autostart();
    config_parse_accessibility();
    config_parse_rules();
    
    return true;
}

/**
 * @brief Parse a line from the config file
 * 
 * @param line Line to parse
 * @param section Current section being parsed
 * @return true if parsed successfully, false otherwise
 */
static bool
parse_config_line(char *line, char *section)
{
    /* Skip empty lines and comments */
    trim(line);
    if (line[0] == '\0' || line[0] == '#')
        return true;
    
    /* Check if this is a section header */
    if (is_section_start(line))
        return parse_section_header(line, section);
    
    /* Parse key-value pair */
    char key[128] = "";
    char value[896] = "";
    
    if (!parse_key_value(line, key, value))
        return false;
    
    /* Process based on current section */
    if (strcmp(section, "appearance") == 0) {
        if (strcmp(key, "border_width") == 0)
            dina_config.border_width = atoi(value);
        else if (strcmp(key, "snap_distance") == 0)
            dina_config.snap_distance = atoi(value);
        else if (strcmp(key, "show_bar") == 0)
            dina_config.show_bar = (strcmp(value, "true") == 0);
        else if (strcmp(key, "font") == 0)
            strncpy(dina_config.font, value, sizeof(dina_config.font) - 1);
    }
    else if (strcmp(section, "layout") == 0) {
        if (strcmp(key, "resize_hints") == 0)
            dina_config.resize_hints = (strcmp(value, "true") == 0);
        else if (strcmp(key, "lock_fullscreen") == 0)
            dina_config.lock_fullscreen = (strcmp(value, "true") == 0);
    }
    
    /* Other sections are processed in post-processing functions */
    
    return true;
}

/**
 * @brief Check if a line is a section start
 */
static bool
is_section_start(const char *line)
{
    return (line[0] == '[' && strchr(line, ']') != NULL);
}

/**
 * @brief Parse a section header from a line
 * 
 * @param line Line to parse
 * @param section Output for section name
 * @return true if parsed successfully, false otherwise
 */
static bool
parse_section_header(const char *line, char *section)
{
    char *end;
    
    /* Extract section name */
    if (line[0] != '[')
        return false;
    
    end = strchr(line, ']');
    if (!end)
        return false;
    
    /* Copy section name without brackets */
    int len = end - line - 1;
    if (len >= 64)
        len = 63;  /* Prevent overflow */
    
    strncpy(section, line + 1, len);
    section[len] = '\0';
    
    return true;
}

/**
 * @brief Parse a key-value pair
 * 
 * @param line Line to parse
 * @param key Output for key
 * @param value Output for value
 * @return true if parsed successfully, false otherwise
 */
static bool
parse_key_value(const char *line, char *key, char *value)
{
    const char *equals = strchr(line, '=');
    if (!equals)
        return false;
    
    /* Extract key */
    int key_len = equals - line;
    if (key_len >= 128)
        key_len = 127;  /* Prevent overflow */
    
    strncpy(key, line, key_len);
    key[key_len] = '\0';
    trim(key);
    
    /* Extract value */
    strncpy(value, equals + 1, 895);
    value[895] = '\0';
    trim(value);
    
    /* Remove quotes if present */
    if (value[0] == '"' && value[strlen(value) - 1] == '"') {
        memmove(value, value + 1, strlen(value) - 2);
        value[strlen(value) - 2] = '\0';
    }
    
    return true;
}

/**
 * @brief Trim whitespace from a string
 */
static void
trim(char *str)
{
    char *start = str;
    char *end;
    
    /* Skip leading whitespace */
    while (isspace((unsigned char)*start))
        start++;
    
    /* If string is only whitespace */
    if (*start == '\0') {
        *str = '\0';
        return;
    }
    
    /* Find end of string */
    end = start + strlen(start) - 1;
    
    /* Trim trailing whitespace */
    while (end > start && isspace((unsigned char)*end))
        end--;
    
    /* Write new null terminator */
    *(end + 1) = '\0';
    
    /* Move if needed */
    if (start != str)
        memmove(str, start, (end - start) + 2);
}

/**
 * @brief Parse key symbol from string
 */
KeySym
config_parse_key(const char *key_str)
{
    /* Handle standard keys */
    if (strlen(key_str) == 1)
        return XStringToKeysym(key_str);
    
    /* Handle special keys */
    if (strcmp(key_str, "Return") == 0)
        return XK_Return;
    else if (strcmp(key_str, "Escape") == 0)
        return XK_Escape;
    else if (strcmp(key_str, "BackSpace") == 0)
        return XK_BackSpace;
    else if (strcmp(key_str, "Tab") == 0)
        return XK_Tab;
    else if (strcmp(key_str, "space") == 0)
        return XK_space;
    else if (strcmp(key_str, "minus") == 0)
        return XK_minus;
    else if (strcmp(key_str, "equal") == 0)
        return XK_equal;
    else if (strcmp(key_str, "Left") == 0)
        return XK_Left;
    else if (strcmp(key_str, "Right") == 0)
        return XK_Right;
    else if (strcmp(key_str, "Up") == 0)
        return XK_Up;
    else if (strcmp(key_str, "Down") == 0)
        return XK_Down;
    
    /* Try direct X key lookup */
    return XStringToKeysym(key_str);
}

/**
 * @brief Parse modifier key from string
 */
unsigned int
config_parse_mod(const char *mod_str)
{
    if (strcmp(mod_str, "shift") == 0)
        return ShiftMask;
    else if (strcmp(mod_str, "control") == 0 || strcmp(mod_str, "ctrl") == 0)
        return ControlMask;
    else if (strcmp(mod_str, "alt") == 0)
        return Mod1Mask;
    else if (strcmp(mod_str, "super") == 0)
        return Mod4Mask;
    
    return 0;
}

/**
 * @brief Parse tag from string
 */
unsigned int
config_parse_tag(const char *tag_str)
{
    int tag = atoi(tag_str);
    if (tag >= 1 && tag <= 9)
        return 1 << (tag - 1);
    return 0;
}

/**
 * @brief Parse keybindings section
 */
static void
config_parse_keybindings(void)
{
    /* We now include keys.h, so this isn't needed */
    
    /* Process keybindings from configuration */
    int i;
    for (i = 0; i < dina_config.num_keybinds; i++) {
        DINAKeybind *kb = &dina_config.keybinds[i];
        
        /* Parse keybinding */
        if (strlen(kb->action) > 0) {
            /* Check if it's an exec command */
            if (strncmp(kb->action, "exec:", 5) == 0) {
                /* Execute external command */
                register_keybinding(kb->modifier | kb->additional_mod, 
                                   kb->keysym, MODE_NORMAL,
                                   NULL, NULL, kb->action + 5);
            } else {
                /* TODO: Handle internal actions like focus_next, etc. */
                /* This part requires mapping action names to functions */
                fprintf(stderr, "DINA: Unhandled action: %s\n", kb->action);
            }
        }
    }
}

/**
 * @brief Parse appearance section
 */
static void
config_parse_appearance(void)
{
    /* Already processed in main parsing loop */
}

/**
 * @brief Parse layout section
 */
static void
config_parse_layout(void)
{
    /* Already processed in main parsing loop */
}

/**
 * @brief Parse workspaces section
 */
static void
config_parse_workspaces(void)
{
    /* Tag names will be set from configuration file */
    /* This will be implemented in a later phase */
}

/**
 * @brief Parse autostart section
 */
static void
config_parse_autostart(void)
{
    /* Autostart programs will be parsed from configuration file */
    /* This will be implemented in a later phase */
}

/**
 * @brief Parse accessibility section
 */
static void
config_parse_accessibility(void)
{
    /* Accessibility settings will be parsed from configuration file */
    /* This will be implemented in a later phase */
}

/**
 * @brief Parse rules section
 */
static void
config_parse_rules(void)
{
    /* Window rules will be parsed from configuration file */
    /* This will be implemented in a later phase */
}

/* The str_replace function has been removed as it's not being used */

/**
 * @brief Create default configuration file
 */
void
dina_config_create_default(void)
{
    FILE *f;
    char path[512];
    char dir_path[512];
    char *home;
    
    /* Get path to config file */
    home = getenv("HOME");
    if (!home)
        return;  /* Can't get home directory */
    
    snprintf(path, sizeof(path), "%s%s", home, CONFIG_PATH);
    
    /* Create directories if needed */
    snprintf(dir_path, sizeof(dir_path), "%s/.config", home);
    mkdir(dir_path, 0755);
    snprintf(dir_path, sizeof(dir_path), "%s/.config/dina", home);
    mkdir(dir_path, 0755);
    
    f = fopen(path, "w");
    if (!f)
        return;  /* Can't write file */
    
    /* Write default configuration */
    fprintf(f, "# DINA Window Manager Configuration\n");
    fprintf(f, "# Auto-generated default configuration\n\n");
    
    fprintf(f, "[appearance]\n");
    fprintf(f, "border_width = 1\n");
    fprintf(f, "snap_distance = 32\n");
    fprintf(f, "show_bar = false\n");
    fprintf(f, "font = \"monospace:size=10\"\n\n");
    
    fprintf(f, "[layout]\n");
    fprintf(f, "default_layout = \"monocle\"\n");
    fprintf(f, "resize_hints = false\n");
    fprintf(f, "lock_fullscreen = true\n\n");
    
    fprintf(f, "[workspaces]\n");
    fprintf(f, "tags = [\"1\", \"2\", \"3\", \"4\", \"5\", \"6\", \"7\", \"8\", \"9\"]\n");
    fprintf(f, "tag_names = [\n");
    fprintf(f, "  \"Main\", \n");
    fprintf(f, "  \"Web\", \n");
    fprintf(f, "  \"Code\", \n");
    fprintf(f, "  \"Media\", \n");
    fprintf(f, "  \"Chat\", \n");
    fprintf(f, "  \"Files\", \n");
    fprintf(f, "  \"Seven\", \n");
    fprintf(f, "  \"Eight\", \n");
    fprintf(f, "  \"Nine\"\n");
    fprintf(f, "]\n\n");
    
    fprintf(f, "[keybindings]\n");
    fprintf(f, "modifier = \"super\"\n\n");
    fprintf(f, "# Navigation\n");
    fprintf(f, "\"modifier+j\" = \"focus_previous\"\n");
    fprintf(f, "\"modifier+l\" = \"focus_next\"\n");
    fprintf(f, "\"modifier+k\" = \"close_window\"\n");
    fprintf(f, "\"modifier+q+shift\" = \"quit\"\n\n");
    
    fprintf(f, "# Workspace switching\n");
    fprintf(f, "\"modifier+1\" = \"view_tag_1\"\n");
    fprintf(f, "\"modifier+2\" = \"view_tag_2\"\n");
    fprintf(f, "\"modifier+3\" = \"view_tag_3\"\n");
    fprintf(f, "\"modifier+4\" = \"view_tag_4\"\n");
    fprintf(f, "\"modifier+5\" = \"view_tag_5\"\n");
    fprintf(f, "\"modifier+6\" = \"view_tag_6\"\n");
    fprintf(f, "\"modifier+7\" = \"view_tag_7\"\n");
    fprintf(f, "\"modifier+8\" = \"view_tag_8\"\n");
    fprintf(f, "\"modifier+9\" = \"view_tag_9\"\n\n");
    
    fprintf(f, "# Move windows to workspaces\n");
    fprintf(f, "\"modifier+1+shift\" = \"move_to_tag_1\"\n");
    fprintf(f, "\"modifier+2+shift\" = \"move_to_tag_2\"\n");
    fprintf(f, "\"modifier+3+shift\" = \"move_to_tag_3\"\n");
    fprintf(f, "\"modifier+4+shift\" = \"move_to_tag_4\"\n");
    fprintf(f, "\"modifier+5+shift\" = \"move_to_tag_5\"\n");
    fprintf(f, "\"modifier+6+shift\" = \"move_to_tag_6\"\n");
    fprintf(f, "\"modifier+7+shift\" = \"move_to_tag_7\"\n");
    fprintf(f, "\"modifier+8+shift\" = \"move_to_tag_8\"\n");
    fprintf(f, "\"modifier+9+shift\" = \"move_to_tag_9\"\n\n");
    
    fprintf(f, "# External commands\n");
    fprintf(f, "\"modifier+Return\" = \"exec:alacritty\"\n");
    fprintf(f, "\"modifier+a\" = \"exec:~/.local/bin/app-launcher\"\n");
    fprintf(f, "\"modifier+b\" = \"exec:~/.local/bin/toggle-blackout\"\n");
    fprintf(f, "\"modifier+BackSpace\" = \"exec:~/.local/bin/session-menu\"\n\n");
    
    fprintf(f, "[autostart]\n");
    fprintf(f, "# Programs to start when DINA launches\n");
    fprintf(f, "orca = \"~/.local/bin/start-orca\"\n");
    fprintf(f, "# compositor = \"picom --daemon\"\n");
    fprintf(f, "# keyboard = \"setxkbmap -option ctrl:nocaps\"\n\n");
    
    fprintf(f, "[accessibility]\n");
    fprintf(f, "# Sound/speech preferences for various actions\n");
    fprintf(f, "enable_sounds = true\n");
    fprintf(f, "sound_directory = \"~/.local/share/dina/sounds/\"\n\n");
    
    fprintf(f, "# Sounds for different actions\n");
    fprintf(f, "sounds = {\n");
    fprintf(f, "  startup = \"startup.wav\",\n");
    fprintf(f, "  workspace_change = \"workspace.wav\",\n");
    fprintf(f, "  window_close = \"close.wav\",\n");
    fprintf(f, "  window_focus = \"focus.wav\",\n");
    fprintf(f, "  error = \"error.wav\"\n");
    fprintf(f, "}\n\n");
    
    fprintf(f, "# Screen reader integration\n");
    fprintf(f, "enable_screen_reader = true\n");
    fprintf(f, "announce_window_changes = true\n");
    fprintf(f, "announce_workspace_changes = true\n");
    fprintf(f, "speech_rate = 1.0\n");
    fprintf(f, "speech_pitch = 0.0\n\n");
    
    fprintf(f, "[rules]\n");
    fprintf(f, "# Window placement rules\n");
    fprintf(f, "# Format: \"class:instance:title\" = { tags = \"tag_list\", floating = true/false, monitor = monitor_number }\n");
    fprintf(f, "\"Firefox:*:*\" = { tags = \"2\", floating = false, monitor = 0 }\n");
    fprintf(f, "\"Alacritty:*:*\" = { tags = \"1\", floating = false, monitor = 0 }\n");
    
    fclose(f);
}

/**
 * @brief Run autostart programs
 */
void
config_run_autostart(void)
{
    int i;
    char *cmd;
    
    /* Execute each autostart command */
    for (i = 0; i < dina_config.num_autostart; i++) {
        cmd = expand_home(dina_config.autostart[i].command);
        if (cmd) {
            fprintf(stderr, "Autostarting: %s\n", cmd);
            
            /* Use advanced keybinding's execute_command */
            execute_command(cmd);
            
            free(cmd);
        }
    }
}

/**
 * @brief Apply configuration to DINA
 */
void
dina_config_apply(void)
{
    /* Apply accessibility settings */
    if (dina_config.speech.enabled) {
        char cmd[256];
        snprintf(cmd, sizeof(cmd), "spd-say -r %.0f -p %.0f \"DINA started\"", 
                 dina_config.speech.rate * 100, 
                 dina_config.speech.pitch * 100);
        
        /* Use our execute_command function */
        execute_command(cmd);
    }
    
    /* Apply sound settings */
    if (dina_config.sound.enabled) {
        char cmd[256];
        char *sound_path = expand_home(dina_config.sound.sound_dir);
        
        if (sound_path) {
            snprintf(cmd, sizeof(cmd), "play -q %s/%s &", 
                     sound_path, 
                     dina_config.sound.startup_sound);
            
            /* Use our execute_command function */
            execute_command(cmd);
            
            free(sound_path);
        }
    }
    
    /* Register keyboard shortcuts from config */
config_parse_keybindings();
}