#ifndef ERROR_H
#define ERROR_H

void log(const char *format, ...);
void warning(const char *format, ...);
int error(const char *format, ...);
int error_errno(const char *format, ...);
void fatal(const char *format, ...);
void fatal_errno(const char *format, ...);

#endif
