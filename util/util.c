/**
 * @file util.c
 * @brief Utility functions implementation
 * 
 * This file contains implementations of various utility functions used throughout DINA.
 */

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "util.h"

/**
 * @brief Die with an error message
 * 
 * Print an error message and exit
 * 
 * @param fmt Format string
 * @param ... Format arguments
 */
void
die(const char *fmt, ...)
{
    va_list ap;
    int saved_errno;

    saved_errno = errno;

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    if (fmt[0] && fmt[strlen(fmt)-1] == ':')
        fprintf(stderr, " %s", strerror(saved_errno));
    fputc('\n', stderr);

    exit(1);
}

/**
 * @brief Allocate memory or die
 * 
 * Allocate memory with error checking
 * 
 * @param nmemb Number of elements
 * @param size Size of each element
 * @return Pointer to allocated memory
 */
void *
ecalloc(size_t nmemb, size_t size)
{
    void *p;

    if (!(p = calloc(nmemb, size)))
        die("calloc:");
    return p;
}

/**
 * @brief Debug log
 * 
 * Print a debug message with timestamp
 * 
 * @param fmt Format string
 * @param ... Format arguments
 */
void
debug(const char *fmt, ...)
{
#ifdef DEBUG
    va_list ap;
    FILE *f;
    time_t t;
    struct tm *tm;
    char timestamp[64];

    t = time(NULL);
    tm = localtime(&t);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm);

    f = fopen("dina-debug.log", "a");
    if (!f)
        return;

    fprintf(f, "[%s] ", timestamp);
    
    va_start(ap, fmt);
    vfprintf(f, fmt, ap);
    va_end(ap);
    
    fprintf(f, "\n");
    fclose(f);
#endif
}

/**
 * @brief Format and execute a shell command
 * 
 * @param fmt Format string
 * @param ... Format arguments
 */
void
spawn_cmd(const char *fmt, ...)
{
    va_list ap;
    char cmd[512];
    
    va_start(ap, fmt);
    vsnprintf(cmd, sizeof(cmd), fmt, ap);
    va_end(ap);
    
    if (fork() == 0) {
        if (fork() == 0) {
            setsid();
            execl("/bin/sh", "/bin/sh", "-c", cmd, NULL);
            exit(0);
        }
        exit(0);
    }
    
    wait(NULL);
}

/**
 * @brief Check if file exists
 * 
 * @param path Path to file
 * @return 1 if file exists, 0 otherwise
 */
int
file_exists(const char *path)
{
    struct stat st;
    return (stat(path, &st) == 0);
}

/**
 * @brief Create directories recursively
 * 
 * @param path Path to create
 * @param mode Directory permissions
 * @return 0 on success, -1 on error
 */
int
mkpath(const char *path, mode_t mode)
{
    char tmp[1024];
    char *p = NULL;
    size_t len;
    int ret;

    if (path == NULL || *path == '\0')
        return -1;

    snprintf(tmp, sizeof(tmp), "%s", path);
    len = strlen(tmp);
    
    /* Remove trailing slash */
    if (tmp[len - 1] == '/')
        tmp[len - 1] = '\0';
    
    /* Check if path exists */
    if (file_exists(tmp))
        return 0;
    
    /* Create parent dirs */
    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            ret = mkdir(tmp, mode);
            if (ret == -1 && errno != EEXIST)
                return -1;
            *p = '/';
        }
    }
    
    /* Create final dir */
    ret = mkdir(tmp, mode);
    if (ret == -1 && errno != EEXIST)
        return -1;
    
    return 0;
}