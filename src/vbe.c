#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "compat.h"
#include "dpmi.h"
#include "error.h"
#include "vbe.h"

#define VBE_INT                         0x10

#define VBE_INFO_BLOCK_SIZE             512
#define VBE_MODE_INFO_BLOCK_SIZE        256

PACKED_STRUCT vbe_info_block {
    uint32_t vbe_signature;
    uint16_t vbe_version;
    uint32_t oem_string_ptr;
    uint32_t capabilities;
    uint32_t video_mode_ptr;
    uint16_t total_memory;
    uint16_t oem_software_rev;
    uint32_t oem_vendor_name_ptr;
    uint32_t oem_product_name_ptr;
    uint32_t oem_product_rev_ptr;
    uint8_t reserved[222];
    uint8_t oem_data[256];
};

PACKED_STRUCT vbe_mode_info_block {
    uint8_t data[VBE_MODE_INFO_BLOCK_SIZE];
};

static inline int vbe_call_function(unsigned int fn, struct dpmi_rm_info *rmi)
{
    rmi->eax = fn;

    if (dpmi_simulate_rm_interrupt(VBE_INT, rmi) != 0)
        return -1;

    if ((rmi->eax & 0xFF00) != 0)
        return error(0, "VBE function %02Xh failed", fn);

    return 0;
}

int vbe_get_info(struct vbe_info *info)
{
    static_assert(sizeof(struct vbe_info_block) == VBE_INFO_BLOCK_SIZE,
                  "Packed structure has wrong size");

    struct dpmi_rm_info rmi;
    uint16_t rm_seg, rm_sel;
    struct vbe_info_block *ib;

    if (dpmi_allocate_dos_memory(sizeof(*ib), &rm_seg, &rm_sel) != 0)
        return -1;

    ib = dpmi_ptr_to_rm_segment(rm_seg);
    memset(ib, 0, sizeof(*ib));
    ib->vbe_signature = 'VBE2';

    memset(&rmi, 0, sizeof(rmi));
    rmi.es = rm_seg;

    if (vbe_call_function(0x4F00, &rmi) != 0) {
        dpmi_free_dos_memory(rm_sel);
        return -1;
    }

    info->vbe_signature = ib->vbe_signature;
    info->vbe_version = ib->vbe_version;
    info->capabilities = ib->capabilities;
    info->total_memory = ib->total_memory;
    info->oem_software_rev = ib->oem_software_rev;

    /* Copy video mode info to reserved area. */

    uint16_t *p = dpmi_ptr_to_rm_address(ib->video_mode_ptr);
    uint16_t *q = (uint16_t *) info->reserved;

    while (*p != 0xFFFF)
        *q++ = *p++;

    *q = *p;

    info->video_modes = (uint16_t *) info->reserved;

    /* Copy OEM strings to OEM data area. */

    char *s = info->oem_data;

    info->oem_string = s;
    s = stpcpy(s, (char *) dpmi_ptr_to_rm_address(ib->oem_string_ptr)) + 1;

    info->oem_vendor_name = s;
    s = stpcpy(s, (char *) dpmi_ptr_to_rm_address(ib->oem_vendor_name_ptr)) + 1;

    info->oem_product_name = s;
    s = stpcpy(s, (char *) dpmi_ptr_to_rm_address(ib->oem_product_name_ptr)) + 1;

    info->oem_product_rev = s;
    stpcpy(s, (char *) dpmi_ptr_to_rm_address(ib->oem_product_rev_ptr)) + 1;

    if (dpmi_free_dos_memory(rm_sel) != 0)
        return -1;

    return 0;
}

void vbe_destroy_info(struct vbe_info *info)
{
    free(info->oem_string);
    free(info->oem_vendor_name);
    free(info->oem_product_name);
    free(info->oem_product_rev);
}

int vbe_get_mode_info(unsigned int mode, struct vbe_mode_info *info)
{
    static_assert(sizeof(struct vbe_mode_info_block) == VBE_MODE_INFO_BLOCK_SIZE,
                  "Packed structure has wrong size");

    static_assert(offsetof(struct vbe_mode_info, phys_base_ptr) == 40,
                  "Wrong alignment of structure members");

    struct dpmi_rm_info rmi;
    uint16_t rm_seg, rm_sel;
    struct vbe_mode_info_block *ib;

    if (dpmi_allocate_dos_memory(sizeof(*ib), &rm_seg, &rm_sel) != 0)
        return -1;

    ib = dpmi_ptr_to_rm_segment(rm_seg);
    memset(ib, 0, sizeof(*ib));

    memset(&rmi, 0, sizeof(rmi));
    rmi.ecx = mode;
    rmi.es = rm_seg;

    if (vbe_call_function(0x4F01, &rmi) != 0) {
        dpmi_free_dos_memory(rm_sel);
        return -1;
    }

    memcpy(info, ib, sizeof(*info));

    if (dpmi_free_dos_memory(rm_sel) != 0)
        return -1;

    return 0;
}

int vbe_set_mode(unsigned int mode, unsigned int flags)
{
    struct dpmi_rm_info rmi;

    memset(&rmi, 0, sizeof(rmi));
    rmi.ebx = (mode | flags) & 0xC1FF;

    return vbe_call_function(0x4F02, &rmi);
}

int vbe_get_mode(unsigned int *mode, unsigned int *flags)
{
    struct dpmi_rm_info rmi;

    memset(&rmi, 0, sizeof(rmi));

    if (vbe_call_function(0x4F03, &rmi) != 0)
        return -1;

    *mode = rmi.ebx & 0x3FFF;
    *flags = rmi.ebx & 0xC000;
    return 0;
}

int vbe_save_state(unsigned int flags, uint32_t *handle)
{
    struct dpmi_rm_info rmi;

    memset(&rmi, 0, sizeof(rmi));
    rmi.edx = 0x00;
    rmi.ecx = flags & 0x0F;

    if (vbe_call_function(0x4F04, &rmi) != 0)
        return -1;

    uint16_t rm_seg, rm_sel;
    uint32_t rm_size = rmi.ebx << 6;

    if (dpmi_allocate_dos_memory(rm_size, &rm_seg, &rm_sel) != 0)
        return -1;

    memset(&rmi, 0, sizeof(rmi));
    rmi.edx = 0x01;
    rmi.ecx = flags & 0x0F;
    rmi.es = rm_seg;

    if (vbe_call_function(0x4F04, &rmi) != 0) {
        dpmi_free_dos_memory(rm_sel);
        return -1;
    }

    *handle = rm_sel;
    return 0;
}

int vbe_restore_state(unsigned int flags, uint32_t handle)
{
    struct dpmi_rm_info rmi;

    memset(&rmi, 0, sizeof(rmi));
    rmi.edx = 0x02;
    rmi.ecx = flags & 0x0F;
    rmi.es = (uint16_t) handle;

    return vbe_call_function(0x4F04, &rmi);
}

int vbe_free_state(uint32_t handle)
{
    return dpmi_free_dos_memory((uint16_t) handle);
}

int vbe_get_logical_scanline_length(unsigned int *bytes_per_scanline,
                                    unsigned int *pixels_per_scanline)
{
    struct dpmi_rm_info rmi;

    memset(&rmi, 0, sizeof(rmi));
    rmi.ebx = 0x01;

    if (vbe_call_function(0x4F06, &rmi) != 0)
        return -1;

    if (bytes_per_scanline != NULL)
        *bytes_per_scanline = rmi.ebx;
    if (pixels_per_scanline != NULL)
        *pixels_per_scanline = rmi.ecx;

    return 0;
}

int vbe_get_maximum_scanline_length(unsigned int *bytes_per_scanline,
                                    unsigned int *pixels_per_scanline)
{
    struct dpmi_rm_info rmi;

    memset(&rmi, 0, sizeof(rmi));
    rmi.ebx = 0x03;

    if (vbe_call_function(0x4F06, &rmi) != 0)
        return -1;

    if (bytes_per_scanline != NULL)
        *bytes_per_scanline = rmi.ebx;
    if (pixels_per_scanline != NULL)
        *pixels_per_scanline = rmi.ecx;

    return 0;
}

int vbe_set_logical_scanline_length(enum vbe_scanline_length unit,
                                    unsigned int length,
                                    unsigned int *bytes_per_scanline,
                                    unsigned int *pixels_per_scanline,
                                    unsigned int *max_scanlines)
{
    assert(unit == VBE_SL_BYTES_PER_SCANLINE ||
           unit == VBE_SL_PIXELS_PER_SCANLINE);

    struct dpmi_rm_info rmi;

    memset(&rmi, 0, sizeof(rmi));
    rmi.ebx = (unit == VBE_SL_BYTES_PER_SCANLINE) ? 0x02 : 0x00;
    rmi.ecx = length & 0xFFFF;

    if (vbe_call_function(0x4F06, &rmi) != 0)
        return -1;

    if (bytes_per_scanline != NULL)
        *bytes_per_scanline = rmi.ebx;
    if (pixels_per_scanline != NULL)
        *pixels_per_scanline = rmi.ecx;
    if (max_scanlines != NULL)
        *max_scanlines = rmi.edx;

    return 0;
}

int vbe_get_display_start(int *pixel, int *scanline)
{
    struct dpmi_rm_info rmi;

    memset(&rmi, 0, sizeof(rmi));
    rmi.ebx = 0x01;

    if (vbe_call_function(0x4F07, &rmi) != 0)
        return -1;

    *pixel = rmi.ecx;
    *scanline = rmi.edx;
    return 0;
}

int vbe_set_display_start(int pixel, int scanline, bool wait_for_retrace)
{
    struct dpmi_rm_info rmi;

    memset(&rmi, 0, sizeof(rmi));
    rmi.ebx = wait_for_retrace << 7;
    rmi.ecx = pixel;
    rmi.edx = scanline;

    return vbe_call_function(0x4F07, &rmi);
}

int vbe_set_palette_data(int start, int count, uint32_t data_rm_ptr,
                         bool wait_for_retrace)
{
    struct dpmi_rm_info rmi;

    memset(&rmi, 0, sizeof(rmi));
    rmi.ebx = wait_for_retrace << 7;
    rmi.ecx = count;
    rmi.edx = start;
    rmi.es = hword(data_rm_ptr);
    rmi.edi = lword(data_rm_ptr);

    return vbe_call_function(0x4F09, &rmi);
}
