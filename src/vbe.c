#include <stdlib.h>
#include <string.h>

#include "compat.h"
#include "dpmi.h"
#include "error.h"
#include "vbe.h"

#define VBE_INT         0x10

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

int vbe_get_info(struct vbe_info *info)
{
    struct dpmi_rm_info rmi;
    uint16_t rm_seg, rm_sel;
    struct vbe_info_block *ib;

    if (dpmi_allocate_dos_memory(sizeof(*ib), &rm_seg, &rm_sel) != 0)
        goto failure;

    ib = dpmi_get_rm_segment_start(rm_seg);
    memset(ib, 0, sizeof(*ib));
    ib->vbe_signature = 'VBE2';

    memset(&rmi, 0, sizeof(rmi));
    rmi.eax = 0x4F00;
    rmi.es = rm_seg;

    if (dpmi_simulate_rm_interrupt(VBE_INT, &rmi) != 0) {
        dpmi_free_dos_memory(rm_sel);
        goto failure;
    }

    if ((rmi.eax & 0xFF00) != 0) {
        dpmi_free_dos_memory(rm_sel);
        goto failure;
    }

    info->vbe_signature = ib->vbe_signature;
    info->vbe_version = ib->vbe_version;
    info->capabilities = ib->capabilities;
    info->video_modes = dpmi_get_rm_ptr(ib->video_mode_ptr);
    info->total_memory = ib->total_memory;
    info->oem_software_rev = ib->oem_software_rev;

    info->oem_string =
        strdup((char *) dpmi_get_rm_ptr(ib->oem_string_ptr));
    info->oem_vendor_name =
        strdup((char *) dpmi_get_rm_ptr(ib->oem_vendor_name_ptr));
    info->oem_product_name =
        strdup((char *) dpmi_get_rm_ptr(ib->oem_product_name_ptr));
    info->oem_product_rev =
        strdup((char *) dpmi_get_rm_ptr(ib->oem_product_rev_ptr));

    if (dpmi_free_dos_memory(rm_sel)) {
        vbe_destroy_info(info);
        goto failure;
    }

    return 0;

failure:

    return error(0, "Obtaining VBE controller information failed");
}

void vbe_destroy_info(struct vbe_info *info)
{
    free(info->oem_string);
    free(info->oem_vendor_name);
    free(info->oem_product_name);
    free(info->oem_product_rev);
}
