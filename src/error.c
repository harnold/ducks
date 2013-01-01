#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

static void vprinterr(const char *prefix, int errnum, const char *format,
                      va_list args)
{
    char msg[1024];

    vsnprintf(msg, sizeof(msg), format, args);

    if (errnum != 0)
        fprintf(stderr, "%s%s: %s\n", prefix, msg, strerror(errnum));
    else
        fprintf(stderr, "%s%s\n", prefix, msg);
}

void warning(int errnum, const char *format, ...)
{
    va_list args;

    fflush(stdout);

    va_start(args, format);
    vprinterr("Warning: ", errnum, format, args);
    va_end(args);
}

int error(int errnum, const char *format, ...)
{
    va_list args;

    fflush(stdout);

    va_start(args, format);
    vprinterr("Error: ", errnum, format, args);
    va_end(args);

    return -1;
}

void error_exit(int errnum, const char *format, ...)
{
    va_list args;

    fflush(stdout);

    va_start(args, format);
    vprinterr("Fatal error: ", errnum, format, args);
    va_end(args);

    exit(EXIT_FAILURE);
}
