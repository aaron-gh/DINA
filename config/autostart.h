/**
 * @file autostart.h
 * @brief Autostart configuration for DINA
 *
 * Header file for runtime autostart configuration
 */

#ifndef __DINA_AUTOSTART_H__
#define __DINA_AUTOSTART_H__

#include <stdbool.h>

/* Function declarations */
bool parse_autostart(const char *section_text);
void run_autostart(void);
bool add_autostart(const char *name, const char *command);
bool remove_autostart(const char *name);

#endif /* __DINA_AUTOSTART_H__ */