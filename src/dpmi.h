#ifndef DPMI_H
#define DPMI_H

#include <stdint.h>

#include "compat.h"

enum {
    DPMI_SUCCESS = 0,
    DPMI_ERROR = -1
};

PACKED_STRUCT dpmi_info {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t reserved;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint16_t flags;
    uint16_t es, ds, fs, gs, ip, cs, sp, ss;
};

int dpmi_map_physical_addr(uint32_t phys_addr, uint32_t size, uint32_t *lin_addr);
int dpmi_unmap_physical_addr(uint32_t lin_addr);
int dpmi_rm_int(uint8_t intno, struct dpmi_info *info);
int dpmi_rm_alloc(uint16_t size, uint16_t *segment, uint16_t *selector);
int dpmi_rm_free(uint16_t selector);

#endif
