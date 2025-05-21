/**
 * @file accessibility.c
 * @brief Accessibility configuration for DINA
 *
 * Implementation of runtime accessibility configuration
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "config.h"
#include "../core/dina.h"

/* Sound player program */
#define SOUND_PLAYER "play"

/**
 * @brief Play a sound file
 * 
 * @param sound_name Name of sound to play (from config)
 */
void
play_sound(const char *sound_name)
{
    char *sound_file = NULL;
    char *sound_path = NULL;
    char *home = NULL;
    char *sound_dir = NULL;
    
    /* Check if sounds are enabled */
    if (!dina_config.sound.enabled)
        return;
    
    /* Get sound file path */
    if (strcmp(sound_name, "startup") == 0)
        sound_file = dina_config.sound.startup_sound;
    else if (strcmp(sound_name, "workspace_change") == 0)
        sound_file = dina_config.sound.workspace_sound;
    else if (strcmp(sound_name, "window_close") == 0)
        sound_file = dina_config.sound.window_close_sound;
    else if (strcmp(sound_name, "window_focus") == 0)
        sound_file = dina_config.sound.window_focus_sound;
    else if (strcmp(sound_name, "error") == 0)
        sound_file = dina_config.sound.error_sound;
    else
        return;  /* Unknown sound */
    
    /* Get sound directory */
    if (dina_config.sound.sound_dir[0] == '~') {
        /* Expand tilde */
        home = getenv("HOME");
        if (!home)
            return;
        
        sound_dir = malloc(strlen(home) + strlen(dina_config.sound.sound_dir));
        if (!sound_dir)
            return;
        
        strcpy(sound_dir, home);
        strcat(sound_dir, dina_config.sound.sound_dir + 1);
    } else {
        sound_dir = strdup(dina_config.sound.sound_dir);
    }
    
    /* Build full path */
    sound_path = malloc(strlen(sound_dir) + strlen(sound_file) + 2);
    if (!sound_path) {
        free(sound_dir);
        return;
    }
    
    strcpy(sound_path, sound_dir);
    
    /* Add trailing slash if needed */
    if (sound_dir[strlen(sound_dir) - 1] != '/')
        strcat(sound_path, "/");
    
    strcat(sound_path, sound_file);
    
    /* Play sound in background */
    if (fork() == 0) {
        setsid();
        /* Redirect stdout/stderr to /dev/null */
        freopen("/dev/null", "w", stdout);
        freopen("/dev/null", "w", stderr);
        
        execl(SOUND_PLAYER, SOUND_PLAYER, sound_path, NULL);
        exit(EXIT_SUCCESS);
    }
    
    free(sound_dir);
    free(sound_path);
}

/**
 * @brief Speak a message using speech synthesizer
 * 
 * @param message Message to speak
 * @param priority Priority of message
 */
void
speak_message(const char *message, int priority)
{
    /* Check if speech is enabled */
    if (!dina_config.speech.enabled)
        return;
    
    /* Use speech-dispatcher to speak message */
    if (fork() == 0) {
        char rate_str[16];
        char pitch_str[16];
        
        setsid();
        /* Redirect stdout/stderr to /dev/null */
        freopen("/dev/null", "w", stdout);
        freopen("/dev/null", "w", stderr);
        
        /* Format rate and pitch parameters */
        snprintf(rate_str, sizeof(rate_str), "-r %g", dina_config.speech.rate);
        snprintf(pitch_str, sizeof(pitch_str), "-p %g", dina_config.speech.pitch);
        
        /* Priority: 0=important, 1=less important */
        if (priority == 0) {
            /* Important message - interrupt current speech */
            execl("spd-say", "spd-say", "-i", rate_str, pitch_str, message, NULL);
        } else {
            /* Less important message - don't interrupt */
            execl("spd-say", "spd-say", rate_str, pitch_str, message, NULL);
        }
        
        exit(EXIT_SUCCESS);
    }
}

/**
 * @brief Announce a window change
 * 
 * @param window_name Window name
 * @param window_class Window class
 */
void
announce_window_change(const char *window_name, const char *window_class)
{
    char message[512];
    
    /* Check if window announcement is enabled */
    if (!dina_config.speech.enabled || !dina_config.speech.announce_window_changes)
        return;
    
    /* Create message */
    if (window_name && window_class)
        snprintf(message, sizeof(message), "Window %s, class %s", window_name, window_class);
    else if (window_name)
        snprintf(message, sizeof(message), "Window %s", window_name);
    else if (window_class)
        snprintf(message, sizeof(message), "Window class %s", window_class);
    else
        strcpy(message, "Unknown window");
    
    /* Speak message */
    speak_message(message, 1);
}

/**
 * @brief Announce a workspace change
 * 
 * @param workspace_num Workspace number (1-9)
 */
void
announce_workspace_change(int workspace_num)
{
    char message[128];
    
    /* Check if workspace announcement is enabled */
    if (!dina_config.speech.enabled || !dina_config.speech.announce_workspace_changes)
        return;
    
    /* Check workspace number */
    if (workspace_num < 1 || workspace_num > 9)
        return;
    
    /* Create message using workspace name if available */
    if (strlen(dina_config.tag_names[workspace_num - 1]) > 0) {
        snprintf(message, sizeof(message), "Workspace %d, %s", 
                workspace_num, dina_config.tag_names[workspace_num - 1]);
    } else {
        snprintf(message, sizeof(message), "Workspace %d", workspace_num);
    }
    
    /* Speak message */
    speak_message(message, 0);
    
    /* Play sound */
    play_sound("workspace_change");
}