#include <i86.h>
#include <string.h>

#include "mouse.h"

#define MOUSE_INT       0x33

void mouse_init()
{
    union REGS regs;

    memset(&regs, 0, sizeof(regs));
    regs.w.ax = 0x00;
    int386(MOUSE_INT, &regs, &regs);
}

void mouse_exit()
{
}

void mouse_get_status(enum mouse_buttons *buttons, int *dx, int *dy)
{
    mouse_get_buttons(buttons);
    mouse_get_delta(dx, dy);
}

void mouse_get_buttons(enum mouse_buttons *buttons)
{
    union REGS regs;

    memset(&regs, 0, sizeof(regs));
    regs.w.ax = 0x03;
    int386(MOUSE_INT, &regs, &regs);
    *buttons = regs.w.bx;
}

void mouse_get_delta(int *dx, int *dy)
{
    union REGS regs;

    memset(&regs, 0, sizeof(regs));
    regs.w.ax = 0x0B;
    int386(MOUSE_INT, &regs, &regs);
    *dx = regs.w.cx;
    *dy = regs.w.dx;
}
