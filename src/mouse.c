#include <i86.h>
#include <string.h>

#include "error.h"
#include "mouse.h"

#define MOUSE_INT       0x33

int mouse_init(void)
{
    union REGS regs;

    memset(&regs, 0, sizeof(regs));
    regs.w.ax = 0x00;
    int386(MOUSE_INT, &regs, &regs);

    if (regs.w.ax != 0xFFFF)
        return error("No mouse driver found");

    return 0;
}

void mouse_exit(void)
{
}

void mouse_get_status(unsigned int *buttons, int *dx, int *dy)
{
    mouse_get_buttons(buttons);
    mouse_get_delta(dx, dy);
}

void mouse_get_buttons(unsigned int *buttons)
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
