#ifndef COMPAT_H
#define COMPAT_H

#include <stdint.h>

#define array_length(a)         (sizeof(a) / sizeof((a)[0]))

static inline uint32_t hword(uint32_t x) { return x >> 16; }
static inline uint32_t lword(uint32_t x) { return x & 0xFFFF; }

static inline char *stpcpy(char *dst, const char *src)
{
    while (*src != '\0')
        *dst++ = *src++;
    *dst = *src;
    return dst;
}

#if defined(__WATCOMC__)

#define PACKED_STRUCT   _Packed struct

#define _Static_assert(cond, msg) \
    ((void) sizeof(struct { int __static_assert_failure: (cond) ? 1 : -1; }))

#ifndef static_assert
#define static_assert _Static_assert
#endif

#else
#error Unsupported compiler
#endif

#endif
