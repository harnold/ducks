#ifndef COMPAT_H
#define COMPAT_H

#define array_length(a)         (sizeof(a) / sizeof((a)[0]))

#if defined(__WATCOMC__)

#define PACKED_STRUCT   _Packed struct

#else
#error Unsupported compiler
#endif

#endif
