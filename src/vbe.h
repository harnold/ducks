#ifndef VBE_H
#define VBE_H

#include <stddef.h>
#include <stdint.h>

enum vbe_capabilities {
    VBE_CAP_DAC_SWITCHABLE_TO_8_BIT             = 1 << 0,
    VBE_CAP_NO_VGA_CONTROLLER                   = 1 << 1,
    VBE_CAP_RAMDAC_REQUIRES_VBLANK              = 1 << 2,
    VBE_CAP_STEREO_SYNC_SUPPORTED               = 1 << 3,
    VBE_CAP_STEREO_SYNC_VIA_EVC_CONNECTOR       = 1 << 4
};

struct vbe_info {
    char vbe_signature[4];
    uint16_t vbe_version;
    char *oem_string;
    uint32_t capabilities;
    uint16_t *video_modes;
    uint16_t total_memory;
    uint16_t oem_software_rev;
    char *oem_vendor_name;
    char *oem_product_name;
    char *oem_product_rev;
};

int vbe_get_info(struct vbe_info *info);
void vbe_destroy_info(struct vbe_info *info);

#endif
