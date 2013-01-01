#include <conio.h>
#include <i86.h>
#include <string.h>

#include "vga.h"

#define VGA_INT                 0x10
#define VGA_DAC_READ_ADDRESS    0x3C7
#define VGA_DAC_WRITE_ADDRESS   0x3C8
#define VGA_DAC_DATA            0x3C9
#define VGA_INPUT_STATUS_1      0x3DA

void vga_get_video_mode(unsigned int *mode)
{
    union REGS regs;

    memset(&regs, 0, sizeof(regs));
    regs.w.ax = 0x0F00;
    int386(VGA_INT, &regs, &regs);
    *mode = regs.h.al;
}

void vga_set_video_mode(unsigned int mode)
{
    union REGS regs;

    memset(&regs, 0, sizeof(regs));
    regs.w.ax = (uint16_t) (mode & 0xFF);
    int386(VGA_INT, &regs, &regs);
}

void vga_set_color(int index, rgb_t rgb)
{
    outp(VGA_DAC_WRITE_ADDRESS, index);
    outp(VGA_DAC_DATA, rgb_r(rgb));
    outp(VGA_DAC_DATA, rgb_g(rgb));
    outp(VGA_DAC_DATA, rgb_b(rgb));
}

void vga_get_color(int index, rgb_t *rgb)
{
    unsigned int r, g, b;

    outp(VGA_DAC_READ_ADDRESS, index);
    r = inp(VGA_DAC_DATA);
    g = inp(VGA_DAC_DATA);
    b = inp(VGA_DAC_DATA);
    *rgb = make_rgb(r, g, b);
}

void vga_set_palette(const struct vga_palette *pal)
{
    const uint8_t *p = pal->data;

    outp(VGA_DAC_WRITE_ADDRESS, 0);
    for (int i = 0; i < VGA_NUM_COLORS; i++) {
        outp(VGA_DAC_DATA, *p++);
        outp(VGA_DAC_DATA, *p++);
        outp(VGA_DAC_DATA, *p++);
    }
}

void vga_wait_for_retrace(void)
{
    while ((inp(VGA_INPUT_STATUS_1) & 8) != 0)
        ;
    while ((inp(VGA_INPUT_STATUS_1) & 8) == 0)
        ;
}
