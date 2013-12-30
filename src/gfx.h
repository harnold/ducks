#ifndef GFX_H
#define GFX_H

#include <stdbool.h>
#include <stddef.h>

struct image;

struct gfx_mode_info {
    int mode;
    int x_resolution;
    int y_resolution;
    size_t page_size;
    double refresh_rate;
    bool vsync_supported:1;
};

int gfx_init(int mode);
void gfx_exit(void);
void gfx_get_mode_info(struct gfx_mode_info *info);
void gfx_set_clip_rect(int x1, int y1, int x2, int y2);
void gfx_reset_clip_rect(void);
bool gfx_clip(int *x, int *y, int *w, int *h);
void gfx_flip(void);

#endif
