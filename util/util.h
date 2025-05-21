/**
 * @file util.h
 * @brief Utility functions for DINA
 *
 * This file contains various utility functions used throughout DINA.
 */

#ifndef _UTIL_H
#define _UTIL_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/* Macro for array length */
#define LENGTH(X) (sizeof(X) / sizeof((X)[0]))

/* Macro for text width */
#define TEXTW(X) (drw_fontset_getwidth(drw, (X)) + lrpad)

/* Window geometry macros */
#define WIDTH(X)  ((X)->w + 2 * (X)->bw)
#define HEIGHT(X) ((X)->h + 2 * (X)->bw)

/* Window visibility macro */
#define ISVISIBLE(C) ((C)->tags & (C)->mon->tagset[(C)->mon->seltags])

/* Macro for maximum of two values */
#define MAX(A, B) ((A) > (B) ? (A) : (B))

/* Macro for minimum of two values */
#define MIN(A, B) ((A) < (B) ? (A) : (B))

/* Macro for absolute value */
#define ABS(X) ((X) < 0 ? -(X) : (X))

/* Macro for cleaning up modifier mask */
#define CLEANMASK(mask) (mask & ~(numlockmask|LockMask) & (ShiftMask|ControlMask|Mod1Mask|Mod2Mask|Mod3Mask|Mod4Mask|Mod5Mask))

/**
 * @brief Allocate memory or die
 * 
 * Allocate memory with error checking
 * 
 * @param size Size of memory to allocate
 * @return Pointer to allocated memory
 */
void *ecalloc(size_t nmemb, size_t size);

/**
 * @brief Die with an error message
 * 
 * Print an error message and exit
 * 
 * @param fmt Format string
 * @param ... Format arguments
 */
void die(const char *fmt, ...);

/**
 * @brief Debug log
 * 
 * Print a debug message
 * 
 * @param fmt Format string
 * @param ... Format arguments
 */
void debug(const char *fmt, ...);

/**
 * @brief Format and execute a shell command
 * 
 * @param fmt Format string
 * @param ... Format arguments
 */
void spawn_cmd(const char *fmt, ...);

/**
 * @brief Check if file exists
 * 
 * @param path Path to file
 * @return 1 if file exists, 0 otherwise
 */
int file_exists(const char *path);

/**
 * @brief Create directories recursively
 * 
 * @param path Path to create
 * @param mode Directory permissions
 * @return 0 on success, -1 on error
 */
int mkpath(const char *path, mode_t mode);

#endif /* _UTIL_H */