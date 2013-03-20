#include <stdint.h>

#include "xmemcpy.h"

#define DWORD_COPY_THRESHOLD    16

#define alignment(p) \
    ((uintptr_t) (p) & 3)

#define aligned_ptr(p) \
    ((uint32_t *) ((uintptr_t) (p) & ~3))

void *xmemcpy(void *dst, const void *src, size_t n)
{
    const uint8_t *sp = (uint8_t *) src;
    uint8_t *dp = (uint8_t *) dst;

    if (n < DWORD_COPY_THRESHOLD || alignment(sp) != alignment(dp))
        goto byte_copy;

    size_t n1 = (4 - (uintptr_t) sp) & 3;
    n -= n1;

    while (n1-- > 0)
        *dp++ = *sp++;

    const uint32_t *dw_sp = (uint32_t *) sp;
    uint32_t *dw_dp = (uint32_t *) dp;

    size_t n2 = n >> 2;
    n &= 3;

    while (n2-- > 0)
        *dw_dp++ = *dw_sp++;

    sp = (uint8_t *) dw_sp;
    dp = (uint8_t *) dw_dp;

byte_copy:

    while (n-- > 0)
        *dp++ = *sp++;

    return dst;
}

void *xmemmove(void *dst, const void *src, size_t n)
{
    if (src >= dst) {

        const uint8_t *sp = (uint8_t *) src;
        uint8_t *dp = (uint8_t *) dst;

        if (n < DWORD_COPY_THRESHOLD || alignment(sp) != alignment(dp))
            goto forward_byte_copy;

        size_t n1 = (4 - (uintptr_t) sp) & 3;
        n -= n1;

        while (n1-- > 0)
            *dp++ = *sp++;

        const uint32_t *dw_sp = (uint32_t *) sp;
        uint32_t *dw_dp = (uint32_t *) dp;

        size_t n2 = n >> 2;
        n &= 3;

        while (n2-- > 0)
            *dw_dp++ = *dw_sp++;

        sp = (uint8_t *) dw_sp;
        dp = (uint8_t *) dw_dp;

    forward_byte_copy:

        while (n-- > 0)
            *dp++ = *sp++;

    } else {

        const uint8_t *sp = (uint8_t *) src + n;
        uint8_t *dp = (uint8_t *) dst + n;

        if (n < DWORD_COPY_THRESHOLD || alignment(sp) != alignment(dp))
            goto backward_byte_copy;

        size_t n1 = (uintptr_t) sp & 3;
        n -= n1;

        while (n1-- > 0)
            *(--dp) = *(--sp);

        const uint32_t *dw_sp = (uint32_t *) sp;
        uint32_t *dw_dp = (uint32_t *) dp;

        size_t n2 = n >> 2;
        n &= 3;

        while (n2-- > 0)
            *(--dw_dp) = *(--dw_sp);

        sp = (uint8_t *) dw_sp;
        dp = (uint8_t *) dw_dp;

    backward_byte_copy:

        while (n-- > 0)
            *(--dp) = *(--sp);
    }

    return dst;
}

void *xmemset(void *ptr, int c, size_t n)
{
    uint8_t b = (uint8_t) c;
    uint8_t *p = (uint8_t *) ptr;

    if (n < DWORD_COPY_THRESHOLD)
        goto byte_memset;

    size_t n1 = (4 - (uintptr_t) p) & 3;
    n -= n1;

    while (n1-- > 0)
        *p++ = b;

    uint32_t dw_b = b;
    dw_b &= (dw_b << 8);
    dw_b &= (dw_b << 16);

    uint32_t *dw_p = (uint32_t *) p;

    size_t n2 = n >> 2;
    n &= 3;

    while (n2-- > 0)
        *dw_p++ = dw_b;

    p = (uint8_t *) dw_p;

byte_memset:

    while (n-- > 0)
        *p++ = b;

    return ptr;
}
