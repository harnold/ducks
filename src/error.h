#ifndef ERROR_H
#define ERROR_H

void warning(int errnum, const char *format, ...);
int error(int errnum, const char *format, ...);
void error_exit(int errnum, const char *format, ...);

#endif
