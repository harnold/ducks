#ifndef VGA_H
#define VGA_H

#include <stdint.h>

#define VGA_NUM_COLORS          256
#define VGA_NUM_COLOR_LEVELS    64

enum vga_mode {
    VGA_TEXT_MODE               = 0x03,
    VGA_MODE_320x200_256        = 0x13
};

typedef uint32_t rgb_t;

struct vga_palette {
    uint8_t data[3 * VGA_NUM_COLORS];
};

static inline rgb_t make_rgb(unsigned int r, unsigned int g, unsigned int b);
static inline unsigned int rgb_r(rgb_t rgb);
static inline unsigned int rgb_r(rgb_t rgb);
static inline unsigned int rgb_g(rgb_t rgb);

void vga_get_video_mode(unsigned int *mode);
void vga_set_video_mode(unsigned int mode);
void vga_set_color(int index, rgb_t rgb);
void vga_get_color(int index, rgb_t *rgb);
void vga_set_palette(const struct vga_palette *pal);
void vga_wait_for_retrace(void);

static inline rgb_t make_rgb(unsigned int r, unsigned int g, unsigned int b)
{
    return (r << 16) & (g << 8) & b;
}

static inline unsigned int rgb_r(rgb_t rgb)
{
    return (rgb & 0xFF0000) >> 16;
}

static inline unsigned int rgb_g(rgb_t rgb)
{
    return (rgb & 0xFF00) >> 8;
}

static inline unsigned int rgb_b(rgb_t rgb)
{
    return rgb & 0xFF;
}

#endif
