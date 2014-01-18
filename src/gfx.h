#ifndef GFX_H
#define GFX_H

#include <stdbool.h>
#include <stddef.h>

enum gfx_flags {
    GFX_NO_CLIPPING = 1 << 16
};

struct image;

struct gfx_mode_info {
    int mode;
    int x_resolution;
    int y_resolution;
    size_t page_size;
    float refresh_rate;
    bool vsync_supported:1;
    bool vsync_requires_vga_wait:1;
};

int gfx_init(int mode);
void gfx_exit(void);
void gfx_get_mode_info(struct gfx_mode_info *info);
void gfx_set_clip_rect(int x, int y, int w, int h);
void gfx_reset_clip_rect(void);
bool gfx_clip(int *x, int *y, int *w, int *h);
void gfx_flip(void);
void gfx_draw_image_section(const struct image *image, int src_x, int src_y,
                            int src_w, int src_h, int dst_x, int dst_y,
                            unsigned flags);
void gfx_draw_image(const struct image *image, int x, int y, unsigned flags);

#endif
