#include "error.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void vprinterr(const char *prefix, int errnum, const char *format,
                      va_list args)
{
    char msg[1024];

    vsnprintf(msg, sizeof(msg), format, args);
    fflush(stdout);

    if (errnum != 0)
        fprintf(stderr, "%s%s: %s\n", prefix, msg, strerror(errnum));
    else
        fprintf(stderr, "%s%s\n", prefix, msg);
}

void warning(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    vprinterr("Warning: ", 0, format, args);
    va_end(args);
}

int error(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    vprinterr("Error: ", 0, format, args);
    va_end(args);

    return -1;
}

int error_errno(const char *format, ...)
{
    int errnum = errno;
    va_list args;

    va_start(args, format);
    vprinterr("Error: ", errnum, format, args);
    va_end(args);

    return -1;
}

void fatal(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    vprinterr("Fatal error: ", 0, format, args);
    va_end(args);

    exit(EXIT_FAILURE);
}

void fatal_errno(const char *format, ...)
{
    int errnum = errno;
    va_list args;

    va_start(args, format);
    vprinterr("Fatal error: ", errnum, format, args);
    va_end(args);

    exit(EXIT_FAILURE);
}
