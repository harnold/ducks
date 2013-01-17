#ifndef COMPAT_H
#define COMPAT_H

#include <stdint.h>

#define array_length(a)         (sizeof(a) / sizeof((a)[0]))

static inline uint32_t hword(uint32_t x) { return x >> 16; }
static inline uint32_t lword(uint32_t x) { return x & 0xFFFF; }

#if defined(__WATCOMC__)

#define PACKED_STRUCT   _Packed struct

#else
#error Unsupported compiler
#endif

#endif
