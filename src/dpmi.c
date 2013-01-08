#include <i86.h>
#include <string.h>

#include "dpmi.h"
#include "error.h"

#define DPMI_INT        0x31

#define HWORD(x)        ((uint32_t) (x) >> 16)
#define LWORD(x)        ((uint32_t) (x) & 0xFFFF)

int dpmi_map_physical_address(uint32_t phys_addr, uint32_t size, uint32_t *lin_addr)
{
    union REGS regs;

    memset(&regs, 0, sizeof(regs));
    regs.w.ax = 0x0800;
    regs.w.bx = HWORD(phys_addr);
    regs.w.cx = LWORD(phys_addr);
    regs.w.si = HWORD(size);
    regs.w.di = LWORD(size);
    int386(DPMI_INT, &regs, &regs);

    if (regs.x.cflag != 0)
        return DPMI_ERROR;

    *lin_addr = ((uint32_t) regs.w.bx << 16 ) | regs.w.cx;
    return DPMI_SUCCESS;
}

int dpmi_unmap_physical_address(uint32_t lin_addr)
{
    union REGS regs;

    memset(&regs, 0, sizeof(regs));
    regs.w.ax = 0x0801;
    regs.w.bx = HWORD(lin_addr);
    regs.w.cx = LWORD(lin_addr);
    int386(DPMI_INT, &regs, &regs);

    if (regs.x.cflag != 0)
        return DPMI_ERROR;

    return DPMI_SUCCESS;
}

int dpmi_allocate_dos_memory(uint32_t size, uint16_t *segment, uint16_t *selector)
{
    union REGS regs;

    if (size > 0xFFFF0) {
        return error(0, "Cannot allocate DOS memory block of more than %u bytes "
                        "(%u bytes requested)", 0xFFFF0, size);
    }

    memset(&regs, 0, sizeof(regs));
    regs.w.ax = 0x0100;
    regs.w.bx = (uint16_t) ((size + 15) >> 4);
    int386(DPMI_INT, &regs, &regs);

    if (regs.x.cflag != 0)
        return DPMI_ERROR;

    *segment  = regs.w.ax;
    *selector = regs.w.dx;

    return DPMI_SUCCESS;
}

int dpmi_free_dos_memory(uint16_t selector)
{
    union REGS regs;

    memset(&regs, 0, sizeof(regs));
    regs.w.ax = 0x0101;
    regs.w.dx = selector;
    int386(DPMI_INT, &regs, &regs);

    if (regs.x.cflag != 0)
        return DPMI_ERROR;

    return DPMI_SUCCESS;
}

int dpmi_simulate_rm_interrupt(unsigned int inum, struct dpmi_rm_info *info)
{
    union REGS regs;
    struct SREGS sregs;

    memset(&regs, 0, sizeof(regs));
    memset(&sregs, 0, sizeof(sregs));
    regs.w.ax = 0x0300;
    regs.h.bl = (uint8_t) inum;
    sregs.es = FP_SEG(info);
    regs.x.edi = FP_OFF(info);
    int386x(DPMI_INT, &regs, &regs, &sregs);

    if (regs.x.cflag != 0)
        return DPMI_ERROR;

    return DPMI_SUCCESS;
}
