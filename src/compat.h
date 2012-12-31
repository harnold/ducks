#ifndef COMPAT_H
#define COMPAT_H

#if defined(__WATCOMC__)

#define PACKED_STRUCT   _Packed struct

#else
#error Unsupported compiler
#endif

#endif
