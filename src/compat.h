#ifndef COMPAT_H
#define COMPAT_H

#include <assert.h>
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

#ifndef static_assert

#define __concat1(a, b)                 a##b
#define __concat(a, b)                  __concat1(a, b)
#define __unique_identifier(prefix)     __concat(prefix, __LINE__)

#define _Static_assert(cond, msg) \
        typedef struct { \
            int __static_assert_failure: (cond) ? 1 : -1; \
        } __unique_identifier(__static_assert_);

#define static_assert _Static_assert

#endif /* static_assert */

#if defined(__WATCOMC__)

#define PACKED_STRUCT   _Packed struct

#else
#error Unsupported compiler
#endif

#endif
