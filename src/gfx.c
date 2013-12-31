#include "gfx.h"
#include "dpmi.h"
#include "error.h"
#include "image.h"
#include "vbe.h"
#include "vga.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define REFRESH_RATE_TEST_CYCLES	30

static struct gfx_mode_info gfx_mode_info;
static int gfx_saved_vga_mode;

static uint32_t gfx_framebuffer_address = 0;
static struct image gfx_buffer_1;
static struct image gfx_buffer_2;
static struct image *gfx_front_buffer;
static struct image *gfx_back_buffer;

static int gfx_clip_x;
static int gfx_clip_y;
static int gfx_clip_w;
static int gfx_clip_h;

static int gfx_check_vbe_info(struct vbe_info *info)
{
    if (vbe_get_info(info) != 0)
        return error("Obtaining graphics card and VBE driver information failed");

    if (info->vbe_version < 0x200)
        return error("VBE 2.0 required but driver supports only version %d.%d",
                     info->vbe_version >> 8, info->vbe_version & 0xFF);

    if ((info->capabilities & VBE_NO_VGA_CONTROLLER) != 0)
        return error("Graphics card not VGA compatible");

    return 0;
}

static int gfx_check_vbe_mode_info(struct vbe_info *info, int mode,
                                   struct vbe_mode_info *mode_info)
{
    uint16_t *mp = info->video_modes;

    while (*mp != 0xFFFF) {
        if (*mp == mode)
            break;
        mp++;
    }

    if (*mp == 0xFFFF)
        return error("VBE mode %Xh not supported by graphics card or VBE driver", mode);

    if (vbe_get_mode_info(mode, mode_info) != 0)
        return error("Obtaining mode information for VBE mode %4X failed", mode);

    if ((mode_info->mode_attributes & VBE_MODE_SUPPORTED_BY_HARDWARE) == 0)
        return error("VBE mode %Xh not supported by hardware configuration", mode);

    if ((mode_info->mode_attributes & VBE_MODE_NOT_VGA_COMPATIBLE) != 0)
        return error("VBE mode %Xh not VGA compatible", mode);

    if ((mode_info->mode_attributes & VBE_LINEAR_FRAMEBUFFER_SUPPORTED) == 0)
        return error("Linear framebuffer not supported in VBE mode %Xh", mode);

    if (mode_info->number_of_image_pages < 1)
        return error("Insufficient graphics memory for double buffering");

    return 0;
}

static void gfx_check_refresh_rate(void)
{
    clock_t t1 = clock();

    for (int i = 0; i < REFRESH_RATE_TEST_CYCLES; i++)
        gfx_flip();

    clock_t t2 = clock();

    gfx_mode_info.refresh_rate =
        (double) CLOCKS_PER_SEC * REFRESH_RATE_TEST_CYCLES / (t2 - t1);

    gfx_mode_info.vsync_supported = gfx_mode_info.refresh_rate < 100.0;
}

int gfx_init(int mode)
{
    struct vbe_info info;
    struct vbe_mode_info mode_info;

    if (gfx_check_vbe_info(&info) != 0)
        return -1;

    if (gfx_check_vbe_mode_info(&info, mode, &mode_info) != 0)
        return -1;

    vga_get_mode(&gfx_saved_vga_mode);
    gfx_mode_info.mode = gfx_saved_vga_mode;

    if (vbe_set_mode(mode, VBE_LINEAR_FRAMEBUFFER | VBE_CLEAR_DISPLAY_MEMORY) != 0) {
        error("Setting VBE mode %Xh failed", mode);
        goto failure;
    }

    gfx_mode_info.mode = mode;
    gfx_mode_info.x_resolution = mode_info.x_resolution;
    gfx_mode_info.y_resolution = mode_info.y_resolution;
    gfx_mode_info.page_size = mode_info.bytes_per_scanline * mode_info.y_resolution;

    if (mode_info.phys_base_ptr <= 0xFFFFF) {
        error("Unsupported hardware configuration: Framebuffer in real-mode memory "
              "(physical base address = %Xh)", mode_info.phys_base_ptr);
        goto failure;
    }

    if (dpmi_map_physical_address(mode_info.phys_base_ptr,
                                  2 * gfx_mode_info.page_size,
                                  &gfx_framebuffer_address) != 0) {
        error("Mapping graphics card framebuffer to memory failed");
        goto failure;
    }

    if (vbe_set_display_start(0, 0, VBE_WAIT_FOR_RETRACE) != 0) {
        error("Setting display start failed");
        goto failure;
    }

    init_image(&gfx_buffer_1,
               gfx_mode_info.x_resolution,
               gfx_mode_info.y_resolution,
               (uint8_t *) gfx_framebuffer_address);

    init_image(&gfx_buffer_2,
               gfx_mode_info.x_resolution,
               gfx_mode_info.y_resolution,
               gfx_buffer_1.data + gfx_mode_info.page_size);

    gfx_front_buffer = &gfx_buffer_1;
    gfx_back_buffer = &gfx_buffer_2;

    gfx_check_refresh_rate();
    gfx_reset_clip_rect();

    return 0;

failure:

    if (gfx_framebuffer_address != 0) {
        if (dpmi_unmap_physical_address(gfx_framebuffer_address) != 0)
            error("Unmapping graphics card framebuffer failed");
    }

    if (gfx_mode_info.mode != gfx_saved_vga_mode)
        vga_set_mode(gfx_saved_vga_mode);

    return -1;
}

void gfx_exit(void)
{
    vga_set_mode(gfx_saved_vga_mode);

    if (dpmi_unmap_physical_address(gfx_framebuffer_address) != 0)
        error("Unmapping graphics card framebuffer failed");

    gfx_framebuffer_address = 0;
}

void gfx_get_mode_info(struct gfx_mode_info *info)
{
    *info = gfx_mode_info;
}

void gfx_set_clip_rect(int x, int y, int w, int h)
{
    gfx_clip_x = x;
    gfx_clip_y = y;
    gfx_clip_w = w;
    gfx_clip_h = h;
}

void gfx_reset_clip_rect(void)
{
    gfx_clip_x = 0;
    gfx_clip_y = 0;
    gfx_clip_w = gfx_mode_info.x_resolution;
    gfx_clip_h = gfx_mode_info.y_resolution;
}

bool gfx_clip(int *x, int *y, int *w, int *h)
{
    int xs = *x;
    int ys = *y;
    int xe = xs + *w - 1;
    int ye = ys + *h - 1;
    int clip_xe = gfx_clip_x + gfx_clip_w - 1;
    int clip_ye = gfx_clip_y + gfx_clip_h - 1;

    /* Completely inside the clipping rectangle? */

    if (xs >= gfx_clip_x && xe <= clip_xe &&
        ys >= gfx_clip_y && ye <= clip_ye)
        return true;

    /* Completely outside the clipping rectangle? */

    if (xs > clip_xe || xe < gfx_clip_x ||
        ys > clip_ye || ye < gfx_clip_y)
        return false;

    /* Partially inside the clipping rectangle. */

    if (xs < gfx_clip_x)
        xs = gfx_clip_x;
    if (ys < gfx_clip_y)
        ys = gfx_clip_y;
    if (xe > clip_xe)
        xe = clip_xe;
    if (ye > clip_ye)
        ye = clip_ye;

    *x = xs;
    *y = ys;
    *w = xe - xs + 1;
    *h = ye - ys + 1;

    return true;
}

void gfx_flip(void)
{
    if (gfx_front_buffer == &gfx_buffer_1) {
        vbe_set_display_start(0, gfx_mode_info.y_resolution, VBE_WAIT_FOR_RETRACE);
        gfx_front_buffer = &gfx_buffer_2;
        gfx_back_buffer = &gfx_buffer_1;
    } else {
        vbe_set_display_start(0, 0, VBE_WAIT_FOR_RETRACE);
        gfx_front_buffer = &gfx_buffer_1;
        gfx_back_buffer = &gfx_buffer_2;
    }
}

void gfx_draw_image_section(const struct image *src, int src_x, int src_y,
                            int src_w, int src_h, int dst_x, int dst_y,
                            unsigned flags)
{
    if ((flags & GFX_NO_CLIPPING) == 1) {
        image_blit(src, src_x, src_y, src_w, src_h,
                   gfx_back_buffer, dst_x, dst_y, flags & IMAGE_BLIT_MASK);
    } else {
        int cx = dst_x;
        int cy = dst_y;
        int cw = src_w;
        int ch = src_h;

        if (!gfx_clip(&cx, &cy, &cw, &ch))
            return;

        image_blit(src, src_x + (cx - dst_x), src_y + (cy - dst_y), cw, ch,
                   gfx_back_buffer, dst_x, dst_y, flags & IMAGE_BLIT_MASK);
    }
}

void gfx_draw_image(const struct image *src, int dst_x, int dst_y, unsigned flags)
{
    gfx_draw_image_section(src, 0, 0, src->width, src->height, dst_x, dst_y, flags);
}
