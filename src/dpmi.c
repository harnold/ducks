#include <i86.h>
#include <string.h>

#include "dpmi.h"

#define DPMI_INT        0x31

#define HWORD(x)        ((uint16_t) (((x) >> 16) & 0xFFFF))
#define LWORD(x)        ((uint16_t) ((x) & 0xFFFF))

uint32_t dpmi_get_pmptr(uint32_t rmptr)
{
    return ((rmptr & 0xffff0000) >> 12) + (rmptr & 0xffff);
}

int dpmi_alloc_descriptor(uint16_t *selector)
{
    union REGS regs;

    memset(&regs, 0, sizeof(regs));
    regs.w.ax = 0x0000;
    regs.w.cx = 1;
    int386(DPMI_INT, &regs, &regs);

    if (regs.x.cflag)
        return DPMI_ERROR;

    *selector = regs.w.ax;
    return DPMI_SUCCESS;
}

int dpmi_free_descriptor(uint16_t selector)
{
    union REGS regs;

    memset(&regs, 0, sizeof(regs));
    regs.w.ax = 0x0001;
    regs.w.bx = selector;
    int386(DPMI_INT, &regs, &regs);

    if (regs.x.cflag)
        return DPMI_ERROR;

    return DPMI_SUCCESS;
}

int dpmi_get_base_addr(uint16_t selector, uint32_t *lin_addr)
{
    union REGS regs;

    memset(&regs, 0, sizeof(regs));
    regs.w.ax = 0x0006;
    regs.w.bx = selector;
    int386(DPMI_INT, &regs, &regs);

    if (regs.x.cflag)
        return DPMI_ERROR;

    *lin_addr = (uint32_t) regs.w.cx << 16 + regs.w.dx;
    return DPMI_SUCCESS;
}

int dpmi_set_base_addr(uint16_t selector, uint32_t lin_addr)
{
    union REGS regs;

    memset(&regs, 0, sizeof(regs));
    regs.w.ax = 0x0007;
    regs.w.bx = selector;
    regs.w.cx = HWORD(lin_addr);
    regs.w.dx = LWORD(lin_addr);
    int386(DPMI_INT, &regs, &regs);

    if (regs.x.cflag)
        return DPMI_ERROR;

    return DPMI_SUCCESS;
}

int dpmi_set_segment_limit(uint16_t selector, uint32_t limit)
{
    union REGS regs;

    memset(&regs, 0, sizeof(regs));
    regs.w.ax = 0x0008;
    regs.w.bx = selector;
    regs.w.cx = HWORD(limit);
    regs.w.dx = LWORD(limit);
    int386(DPMI_INT, &regs, &regs);

    if (regs.x.cflag)
        return DPMI_ERROR;

    return DPMI_SUCCESS;
}

int dpmi_set_access_rights(uint16_t selector, uint8_t access, uint8_t xaccess)
{
    union REGS regs;

    memset(&regs, 0, sizeof(regs));
    regs.w.ax = 0x0009;
    regs.w.bx = selector;
    regs.h.cl = access;
    regs.h.ch = xaccess;
    int386(DPMI_INT, &regs, &regs);

    if (regs.x.cflag)
        return DPMI_ERROR;

    return DPMI_SUCCESS;
}

int dpmi_map_physical_addr(uint32_t phys_addr, uint32_t size, uint32_t *lin_addr)
{
    union REGS regs;

    memset(&regs, 0, sizeof(regs));
    regs.w.ax = 0x0800;
    regs.w.bx = HWORD(phys_addr);
    regs.w.cx = LWORD(phys_addr);
    regs.w.si = HWORD(size);
    regs.w.di = LWORD(size);
    int386(DPMI_INT, &regs, &regs);

    if (regs.x.cflag)
        return DPMI_ERROR;

    *lin_addr = ((uint32_t) regs.w.bx << 16 ) + regs.w.cx;
    return DPMI_SUCCESS;
}

int dpmi_unmap_physical_addr(uint32_t lin_addr)
{
    union REGS regs;

    memset(&regs, 0, sizeof(regs));
    regs.w.ax = 0x0801;
    regs.w.bx = HWORD(lin_addr);
    regs.w.cx = LWORD(lin_addr);
    int386(DPMI_INT, &regs, &regs);

    if (regs.x.cflag)
        return DPMI_ERROR;

    return DPMI_SUCCESS;
}

int dpmi_rm_int(uint8_t intno, struct dpmi_info *info)
{
    union REGS regs;
    struct SREGS sregs;

    memset(&regs, 0, sizeof(regs));
    memset(&sregs, 0, sizeof(sregs));
    regs.w.ax = 0x0300;
    regs.h.bl = intno;
    sregs.es = FP_SEG(info);
    regs.x.edi = FP_OFF(info);
    int386x(DPMI_INT, &regs, &regs, &sregs);

    if (regs.x.cflag)
        return DPMI_ERROR;

    return DPMI_SUCCESS;
}

int dpmi_rm_alloc(uint16_t size, uint16_t *segment, uint16_t *selector)
{
    union REGS regs;

    memset(&regs, 0, sizeof(regs));
    regs.w.ax = 0x0100;
    regs.w.bx = (uint16_t) ((size + 15) >> 4);
    int386(DPMI_INT, &regs, &regs);

    if (regs.x.cflag)
        return DPMI_ERROR;

    *segment  = regs.w.ax;
    *selector = regs.w.dx;

    return DPMI_SUCCESS;
}

int dpmi_rm_free(uint16_t selector)
{
    union REGS regs;

    memset(&regs, 0, sizeof(regs));
    regs.w.ax = 0x0101;
    regs.w.dx = selector;
    int386(DPMI_INT, &regs, &regs);

    if (regs.x.cflag)
        return DPMI_ERROR;

    return DPMI_SUCCESS;
}
