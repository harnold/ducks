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

PACKED_STRUCT vbe_mode_info_block {
    uint16_t mode_attributes;
    uint8_t win_a_attributes;
    uint8_t win_b_attributes;
    uint16_t win_granularity;
    uint16_t win_size;
    uint16_t win_a_segment;
    uint16_t win_b_segment;
    uint32_t win_func_ptr;
    uint16_t bytes_per_scanline;
    uint16_t x_resolution;
    uint16_t y_resolution;
    uint8_t x_char_size;
    uint8_t y_char_size;
    uint8_t number_of_planes;
    uint8_t bits_per_pixel;
    uint8_t number_of_banks;
    uint8_t memory_model;
    uint8_t bank_size;
    uint8_t number_of_image_pages;
    uint8_t reserved_1;
    uint8_t red_mask_size;
    uint8_t red_field_position;
    uint8_t green_mask_size;
    uint8_t green_field_position;
    uint8_t blue_mask_size;
    uint8_t blue_field_position;
    uint8_t rsvd_mask_size;
    uint8_t rsvd_field_position;
    uint8_t direct_color_mode_info;
    uint32_t phys_base_ptr;
    uint32_t reserved_2;
    uint16_t reserved_3;
    uint8_t reserved_4[17];
    uint8_t reserved_5[189];
};

inline static int vbe_call_function(unsigned int fn, struct dpmi_rm_info *rmi)
{
    rmi->eax = fn;

    if (dpmi_simulate_rm_interrupt(VBE_INT, rmi) != 0 ||
        (rmi->eax & 0xFF00) != 0)
        return -1;

    return 0;
}

int vbe_get_info(struct vbe_info *info)
{
    struct dpmi_rm_info rmi;
    uint16_t rm_seg, rm_sel;
    struct vbe_info_block *ib;

    if (dpmi_allocate_dos_memory(sizeof(*ib), &rm_seg, &rm_sel) != 0)
        goto failure;

    ib = dpmi_ptr_to_rm_segment(rm_seg);
    memset(ib, 0, sizeof(*ib));
    ib->vbe_signature = 'VBE2';

    memset(&rmi, 0, sizeof(rmi));
    rmi.es = rm_seg;

    if (vbe_call_function(0x4F00, &rmi) != 0) {
        dpmi_free_dos_memory(rm_sel);
        goto failure;
    }

    uint32_t *sig = (uint32_t *) info->vbe_signature;
    *sig = ib->vbe_signature;

    info->vbe_version = ib->vbe_version;
    info->capabilities = ib->capabilities;
    info->video_modes = dpmi_ptr_to_rm_address(ib->video_mode_ptr);
    info->total_memory = ib->total_memory;
    info->oem_software_rev = ib->oem_software_rev;

    info->oem_string =
        strdup((char *) dpmi_ptr_to_rm_address(ib->oem_string_ptr));
    info->oem_vendor_name =
        strdup((char *) dpmi_ptr_to_rm_address(ib->oem_vendor_name_ptr));
    info->oem_product_name =
        strdup((char *) dpmi_ptr_to_rm_address(ib->oem_product_name_ptr));
    info->oem_product_rev =
        strdup((char *) dpmi_ptr_to_rm_address(ib->oem_product_rev_ptr));

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

int vbe_get_mode_info(unsigned int mode, struct vbe_mode_info *info)
{
    struct dpmi_rm_info rmi;
    uint16_t rm_seg, rm_sel;
    struct vbe_mode_info_block *ib;

    if (dpmi_allocate_dos_memory(sizeof(*ib), &rm_seg, &rm_sel) != 0)
        goto failure;

    ib = dpmi_ptr_to_rm_segment(rm_seg);
    memset(ib, 0, sizeof(*ib));

    memset(&rmi, 0, sizeof(rmi));
    rmi.ecx = mode;
    rmi.es = rm_seg;

    if (vbe_call_function(0x4F01, &rmi) != 0) {
        dpmi_free_dos_memory(rm_sel);
        goto failure;
    }

    static_assert(offsetof(struct vbe_mode_info, phys_base_ptr) ==
                  offsetof(struct vbe_mode_info_block, phys_base_ptr),
                  "Wrong alignment of structure members");

    memcpy(info, ib, sizeof(*info));

    if (dpmi_free_dos_memory(rm_sel))
        goto failure;

    return 0;

failure:

    return error(0, "Obtaining VBE mode information failed");
}
