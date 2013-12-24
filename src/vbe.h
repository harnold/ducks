#ifndef VBE_H
#define VBE_H

#include <stdbool.h>
#include <stdint.h>

enum vbe_capabilities {
    VBE_DAC_SWITCHABLE_TO_8_BIT         = 1 << 0,
    VBE_NO_VGA_CONTROLLER               = 1 << 1,
    VBE_RAMDAC_REQUIRES_VBLANK          = 1 << 2,
    VBE_STEREO_SYNC_SUPPORTED           = 1 << 3,
    VBE_STEREO_SYNC_VIA_EVC_CONNECTOR   = 1 << 4
};

enum vbe_mode_attributes {
    VBE_MODE_SUPPORTED_BY_HARDWARE              = 1 << 0,
    VBE_BIOS_TTY_OUTPUT_FUNCTIONS_SUPPORTED     = 1 << 2,
    VBE_COLOR_MODE                              = 1 << 3,
    VBE_GRAPHICS_MODE                           = 1 << 4,
    VBE_MODE_NOT_VGA_COMPATIBLE                 = 1 << 5,
    VBE_WINDOWED_FRAMEBUFFER_NOT_SUPPORTED      = 1 << 6,
    VBE_LINEAR_FRAMEBUFFER_SUPPORTED            = 1 << 7,
    VBE_DOUBLE_SCAN_MODE_SUPPORTED              = 1 << 8,
    VBE_INTERLACED_MODE_SUPPORTED               = 1 << 9,
    VBE_TRIPLE_BUFFERING_SUPPORTED              = 1 << 10,
    VBE_STEREO_DISPLAY_SUPPORTED                = 1 << 11,
    VBE_DUAL_DISPLAY_START_ADDRESS_SUPPORTED    = 1 << 12
};

enum vbe_win_attributes {
    VBE_WINDOW_RELOCATABLE              = 1 << 0,
    VBE_WINDOW_READABLE                 = 1 << 1,
    VBE_WINDOW_WRITABLE                 = 1 << 2,
};

enum vbe_memory_model {
    VBE_MEMORY_TEXT_MODE                = 0x00,
    VBE_MEMORY_CGA_GRAPHICS             = 0x01,
    VBE_MEMORY_HERCULES_GRAPHICS        = 0x02,
    VBE_MEMORY_PLANAR                   = 0x03,
    VBE_MEMORY_PACKED_PIXEL             = 0x04,
    VBE_MEMORY_NON_CHAIN_4_256_COLORS   = 0x05,
    VBE_MEMORY_DIRECT_COLOR             = 0x06,
    VBE_MEMORY_YUV                      = 0x07
};

enum vbe_mode {
    VBE_MODE_640x480_256                = 0x101,
    VBE_MODE_800x600_256                = 0x103,
    VBE_MODE_1024x768_256               = 0x105,
    VBE_MODE_1280x1024_256              = 0x107
};

enum vbe_mode_flags {
    VBE_WINDOWED_FRAMEBUFFER            = 0,
    VBE_LINEAR_FRAMEBUFFER              = 1 << 14,
    VBE_CLEAR_DISPLAY_MEMORY            = 0,
    VBE_PRESERVE_DISPLAY_MEMORY         = 1 << 15
};

enum vbe_save_restore_flags {
    VBE_CONTROLLER_STATE                = 1 << 0,
    VBE_BIOS_DATA_STATE                 = 1 << 1,
    VBE_DAC_STATE                       = 1 << 2,
    VBE_REGISTER_STATE                  = 1 << 3,
    VBE_COMPLETE_STATE                  = 0x0F
};

enum vbe_scanline_length {
    VBE_BYTES_PER_SCANLINE,
    VBE_PIXELS_PER_SCANLINE
};

enum {
    VBE_IGNORE_RETRACE,
    VBE_WAIT_FOR_RETRACE
};

struct vbe_info {
    uint32_t vbe_signature;
    uint16_t vbe_version;
    char *oem_string;
    uint32_t capabilities;
    uint16_t *video_modes;
    uint16_t total_memory;
    uint16_t oem_software_rev;
    char *oem_vendor_name;
    char *oem_product_name;
    char *oem_product_rev;
    uint8_t reserved[222];
    uint8_t oem_data[256];
};

struct vbe_mode_info {
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
};

int vbe_get_info(struct vbe_info *info);
int vbe_get_mode_info(int mode, struct vbe_mode_info *mode_info);
int vbe_set_mode(int mode, unsigned flags);
int vbe_get_mode(int *mode, unsigned *flags);
int vbe_save_state(unsigned flags, uint32_t *handle);
int vbe_restore_state(unsigned flags, uint32_t handle);
int vbe_free_state(uint32_t handle);
int vbe_get_logical_scanline_length(unsigned int *bytes_per_scanline,
                                    unsigned int *pixels_per_scanline);
int vbe_get_maximum_scanline_length(unsigned int *bytes_per_scanline,
                                    unsigned int *pixels_per_scanline);
int vbe_set_logical_scanline_length(enum vbe_scanline_length unit,
                                    unsigned int length,
                                    unsigned int *bytes_per_scanline,
                                    unsigned int *pixels_per_scanline,
                                    unsigned int *max_scanlines);
int vbe_get_display_start(int *pixel, int *scanline);
int vbe_set_display_start(int pixel, int scanline, int wait_for_retrace);
int vbe_set_palette_data(int start, int count, uint32_t data_rm_ptr,
                         int wait_for_retrace);

#endif
