#include <stdint.h>

#include "xmemcpy.h"

#define DWORD_COPY_THRESHOLD    16

#define alignment(p) \
    ((uintptr_t) (p) & 3)

#define aligned_ptr(p) \
    ((uint32_t *) ((uintptr_t) (p) & ~3))

void *xmemcpy(void *dst, const void *src, size_t n)
{
    const uint8_t *ep = (uint8_t *) src + n;
    const uint8_t *sp = (uint8_t *) src;
    uint8_t *dp = (uint8_t *) dst;

    if (n < DWORD_COPY_THRESHOLD || alignment(src) != alignment(dst)) {

        while (sp < ep)
            *dp++ = *sp++;

    } else {

        const uint32_t *dw_ep = aligned_ptr(ep);
        const uint32_t *dw_sp = aligned_ptr(sp + 3);
        uint32_t *dw_dp = aligned_ptr(dp + 3);

        while (sp < (uint8_t *) dw_sp)
            *dp++ = *sp++;

        while (dw_sp < dw_ep)
            *dw_dp++ = *dw_sp++;

        sp = (uint8_t *) dw_sp;
        dp = (uint8_t *) dw_dp;

        while (sp < ep)
            *dp++ = *sp++;
    }

    return dst;
}

void *xmemmove(void *dst, const void *src, size_t n)
{
    if (src < dst) {

        const uint8_t *ep = (uint8_t *) src;
        const uint8_t *sp = (uint8_t *) src + n;
        uint8_t *dp = (uint8_t *) dst + n;

        if (n < DWORD_COPY_THRESHOLD || alignment(src) != alignment(dst)) {

            do
                *(--dp) = *(--sp);
            while (sp > ep);

        } else {

            const uint32_t *dw_ep = aligned_ptr(ep + 3);
            const uint32_t *dw_sp = aligned_ptr(sp);
            uint32_t *dw_dp = aligned_ptr(dp);

            do
                *(--dp) = *(--sp);
            while (sp > (uint8_t *) dw_sp);

            do
                *(--dw_dp) = *(--dw_sp);
            while (dw_sp > dw_ep);

            sp = (uint8_t *) dw_sp;
            dp = (uint8_t *) dw_dp;

            do
                *(--dp) = *(--sp);
            while (sp > ep);
        }

    } else {

        const uint8_t *ep = (uint8_t *) src + n;
        const uint8_t *sp = (uint8_t *) src;
        uint8_t *dp = (uint8_t *) dst;

        if (n < DWORD_COPY_THRESHOLD || alignment(src) != alignment(dst)) {

            while (sp < ep)
                *dp++ = *sp++;

        } else {

            const uint32_t *dw_ep = aligned_ptr(ep);
            const uint32_t *dw_sp = aligned_ptr(sp + 3);
            uint32_t *dw_dp = aligned_ptr(dp + 3);

            while (sp < (uint8_t *) dw_sp)
                *dp++ = *sp++;

            while (dw_sp < dw_ep)
                *dw_dp++ = *dw_sp++;

            sp = (uint8_t *) dw_sp;
            dp = (uint8_t *) dw_dp;

            while (sp < ep)
                *dp++ = *sp++;
        }
    }

    return dst;
}

void *xmemset(void *ptr, int c, size_t n)
{
    uint8_t b = (uint8_t) c;
    uint8_t *ep = (uint8_t *) ptr + n;
    uint8_t *p = (uint8_t *) ptr;

    if (n < DWORD_COPY_THRESHOLD) {

        while (p < ep)
            *p++ = b;

    } else {

        uint32_t dw_b = b;
        dw_b &= (dw_b << 8);
        dw_b &= (dw_b << 16);

        uint32_t *dw_ep = aligned_ptr(ep);
        uint32_t *dw_p = aligned_ptr(p + 3);

        while (p < (uint8_t *) dw_p)
            *p++ = b;

        while (dw_p < dw_ep)
            *dw_p++ = dw_b;

        p = (uint8_t *) dw_p;

        while (p < ep)
            *p++ = b;
    }

    return ptr;
}
