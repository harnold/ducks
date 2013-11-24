#include "gfx.h"
#include "dpmi.h"
#include "error.h"
#include "image.h"
#include "vbe.h"
#include "vga.h"
#include "xmemcpy.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define REFRESH_RATE_TEST_CYCLES	30

static struct gfx_mode_info gfx_mode_info;
static int gfx_saved_vga_mode;

static uint32_t gfx_framebuffer_address = 0;
static uint8_t *gfx_buffer_1;
static uint8_t *gfx_buffer_2;
static uint8_t *gfx_front_buffer;
static uint8_t *gfx_back_buffer;

static int gfx_clip_x1;
static int gfx_clip_y1;
static int gfx_clip_x2;
static int gfx_clip_y2;

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

    gfx_buffer_1 = (uint8_t *) gfx_framebuffer_address;
    gfx_buffer_2 = gfx_buffer_1 + gfx_mode_info.page_size;
    gfx_front_buffer = gfx_buffer_1;
    gfx_back_buffer = gfx_buffer_2;

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

void gfx_set_clip_rect(int x1, int y1, int x2, int y2)
{
    gfx_clip_x1 = x1;
    gfx_clip_y1 = y1;
    gfx_clip_x2 = x2;
    gfx_clip_y2 = y2;
}

void gfx_reset_clip_rect(void)
{
    gfx_clip_x1 = 0;
    gfx_clip_y1 = 0;
    gfx_clip_x2 = gfx_mode_info.x_resolution - 1;
    gfx_clip_y2 = gfx_mode_info.y_resolution - 1;
}

void gfx_flip(void)
{
    if (gfx_front_buffer == gfx_buffer_1) {
        vbe_set_display_start(0, gfx_mode_info.y_resolution, VBE_WAIT_FOR_RETRACE);
        gfx_front_buffer = gfx_buffer_2;
        gfx_back_buffer = gfx_buffer_1;
    } else {
        vbe_set_display_start(0, 0, VBE_WAIT_FOR_RETRACE);
        gfx_front_buffer = gfx_buffer_1;
        gfx_back_buffer = gfx_buffer_2;
    }
}

bool gfx_clip(int *x, int *y, int *w, int *h)
{
    int xs = *x;
    int ys = *y;
    int xe = xs + *w - 1;
    int ye = ys + *h - 1;

    /* Completely outside the clipping rectangle? */

    if (xs > gfx_clip_x2 || xe < gfx_clip_x1 ||
        ys > gfx_clip_y2 || ye < gfx_clip_y1)
        return false;

    /* Completely inside the clipping rectangle? */

    if (xs >= gfx_clip_x1 && xe <= gfx_clip_x2 &&
        ys >= gfx_clip_y1 && ye <= gfx_clip_y2)
        return true;

    /* Partially inside the clipping rectangle. */

    if (xs < gfx_clip_x1)
        xs = gfx_clip_x1;
    if (ys < gfx_clip_y1)
        ys = gfx_clip_y1;
    if (xe > gfx_clip_x2)
        xe = gfx_clip_x2;
    if (ye > gfx_clip_y2)
        ye = gfx_clip_y2;

    *x = xs;
    *y = ys;
    *w = xe - xs + 1;
    *h = ye - ys + 1;

    return true;
}

void gfx_blit(uint8_t *src, int src_stride,
              int src_x, int src_y,
              int src_w, int src_h,
              uint8_t *dst, int dst_stride,
              int dst_x, int dst_y)
{
    const uint8_t *sp = src + src_stride * src_y + src_x;
    uint8_t *dp = dst + dst_stride * dst_y + dst_x;

    for (int i = 0; i < src_h; i++) {
        xmemcpy(dp, sp, src_w);
        sp += dst_stride;
        dp += src_stride;
    }
}

void gfx_blit_masked(uint8_t *src, int src_stride,
                     int src_x, int src_y,
                     int src_w, int src_h,
                     uint8_t *dst, int dst_stride,
                     int dst_x, int dst_y)
{
    int src_off = src_stride - src_w;
    int dst_off = dst_stride - src_w;

    const uint8_t *sp = src + src_stride * src_y + src_x;
    uint8_t *dp = dst + dst_stride * dst_y + dst_x;

    for (int i = 0; i < src_h; i++) {

        for (int j = 0; j < src_w; j++) {
            if (*sp != 0) {
                *dp++ = *sp++;
            } else {
                sp++;
                dp++;
            }
        }

        sp += src_off;
        dp += dst_off;
    }
}
