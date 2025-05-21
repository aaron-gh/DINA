/**
 * @file accessibility.h
 * @brief Accessibility configuration for DINA
 *
 * Header file for runtime accessibility configuration
 */

#ifndef __DINA_ACCESSIBILITY_H__
#define __DINA_ACCESSIBILITY_H__

/* Function declarations */
void play_sound(const char *sound_name);
void speak_message(const char *message, int priority);
void announce_window_change(const char *window_name, const char *window_class);
void announce_workspace_change(int workspace_num);

#endif /* __DINA_ACCESSIBILITY_H__ */