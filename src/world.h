#ifndef WORLD_H
#define WORLD_H

#include "gfx.h"

#define WORLD_MIN_X     0.0f
#define WORLD_MAX_X     800.0f
#define WORLD_MIN_Y     0.0f
#define WORLD_MAX_Y     600.0f
#define WORLD_SIZE_X    (WORLD_MAX_X - WORLD_MIN_X)
#define WORLD_SIZE_Y    (WORLD_MAX_Y - WORLD_MIN_Y)

extern struct gfx_mode_info gfx_mode_info;

static inline int world_to_screen_x(float x);
static inline int world_to_screen_dx(float dx);
static inline int world_to_screen_y(float y);
static inline int world_to_screen_dy(float dy);

static inline int world_to_screen_x(float x)
{
    return (int) ((x - WORLD_MIN_X) * gfx_mode_info.x_resolution / WORLD_SIZE_X);
}

static inline int world_to_screen_dx(float dx)
{
    return (int) (dx * gfx_mode_info.x_resolution / WORLD_SIZE_X);
}

static inline int world_to_screen_y(float y)
{
    return (int) ((y - WORLD_MIN_Y) * gfx_mode_info.y_resolution / WORLD_SIZE_Y);
}

static inline int world_to_screen_dy(float dy)
{
    return (int) (dy * gfx_mode_info.y_resolution / WORLD_SIZE_Y);
}

#endif
