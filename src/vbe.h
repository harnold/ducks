#ifndef VBE_H
#define VBE_H

#include <stddef.h>

enum vbe_capabilities {
    VBE_DAC_SWITCHABLE_TO_8_BIT         = 1,
    VBE_NO_VGA_CONTROLLER               = 2,
    VBE_RAMDAC_REQUIRES_VBLANK          = 4,
    VBE_STEREO_SYNC_SUPPORTED           = 8,
    VBE_STEREO_SYNC_VIA_EVC_CONNECTOR   = 16
};

struct vbe_info {
    unsigned int vbe_signature;
    unsigned short vbe_version;
    char *oem_string;
    unsigned int capabilities;
    short *video_modes;
    size_t total_memory;
    unsigned short oem_software_rev;
    char *oem_vendor_name;
    char *oem_product_name;
    char *oem_product_rev;
};

int vbe_get_info(struct vbe_info *info);
void vbe_destroy_info(struct vbe_info *info);

#endif
