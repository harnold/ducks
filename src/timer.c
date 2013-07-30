#include "timer.h"

#include <dos.h>

#define TIMER_INT       0x1C

timer_handler timer_default_handler;

void timer_init(void)
{
    timer_default_handler = _dos_getvect(TIMER_INT);
}

void timer_exit(void)
{
    _dos_setvect(TIMER_INT, timer_default_handler);
}

void timer_set_handler(timer_handler handler)
{
    _dos_setvect(TIMER_INT, handler);
}
