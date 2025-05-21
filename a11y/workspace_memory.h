/**
 * @file workspace_memory.h
 * @brief Workspace (tag) persistence for DINA
 *
 * This file contains the functionality for remembering which applications
 * belong on which workspaces, creating a consistent environment for blind users.
 */

#ifndef _WORKSPACE_MEMORY_H
#define _WORKSPACE_MEMORY_H

#include "core/dina.h"

/**
 * @brief Structure to store application workspace preferences
 */
typedef struct {
    char class[256];     /**< Window class (from WM_CLASS) */
    char instance[256];  /**< Window instance (from WM_CLASS) */
    int tag;             /**< Tag number (1-9) */
} AppPersist;

/**
 * @brief Path to the workspace layout file
 */
#define APP_PERSIST_FILE ".config/dina/workspace_layout"

/**
 * @brief Load application tag persistence data
 * 
 * Read the workspace layout from disk
 */
void loadapptagpersist(void);

/**
 * @brief Save application tag persistence data
 * 
 * Write the workspace layout to disk
 */
void saveapptagpersist(void);

/**
 * @brief Update an application's tag persistence
 * 
 * Store or remove a tag preference for an application
 * 
 * @param class Window class
 * @param instance Window instance
 * @param tag Tag number (1-9)
 */
void updateapptagpersist(const char *class, const char *instance, int tag);

/**
 * @brief Find an application in the persistence store
 * 
 * @param class Window class
 * @param instance Window instance
 * @return Index of the application or -1 if not found
 */
int findapptagpersist(const char *class, const char *instance);

/**
 * @brief Apply workspace memory to a new window
 * 
 * @param c Client to check and apply workspace for
 * @param class Window class
 * @param instance Window instance
 * @return 1 if workspace was applied, 0 otherwise
 */
int applypersistworkspace(Client *c, const char *class, const char *instance);

/**
 * @brief Initialize workspace memory subsystem
 */
void workspace_memory_init(void);

/**
 * @brief Clean up workspace memory resources
 */
void workspace_memory_cleanup(void);

#endif /* _WORKSPACE_MEMORY_H */