#include "game.h"
#include "gfx.h"
#include "image.h"
#include "mouse.h"
#include "palette.h"
#include "res.h"
#include "scene.h"
#include "sprite.h"
#include "timer.h"
#include "vga.h"
#include "world.h"

#include <conio.h>
#include <stdbool.h>

#define KEY_ESC         27

struct gfx_mode_info gfx_mode_info;

static const struct sprite_class pointer_class = {
    &pointer_image, 80, 80, 1, 40.0f, 40.0f
};

static struct scene game_scene;
static struct sprite pointer_sprite;

static inline int confine_int(int x, int min, int max)
{
    return (x < min) ? min : (x > max) ? max : x;
}

static inline float confine_float(float x, float min, float max)
{
    return (x < min) ? min : (x > max) ? max : x;
}

static unsigned update_mouse(void)
{
    unsigned buttons;
    int dx, dy;

    mouse_get_status(&buttons, &dx, &dy);

    pointer_sprite.x =
        confine_float(pointer_sprite.x + dx, 0, gfx_mode_info.x_resolution);
    pointer_sprite.y =
        confine_float(pointer_sprite.y + dy, 0, gfx_mode_info.y_resolution);

    return buttons;
}

int game_init(void)
{
    if (res_load_palettes() != 0)
        return -1;

    if (res_load_images() != 0) {
        res_destroy_images();
        return -1;
    }

    gfx_get_mode_info(&gfx_mode_info);
    vga_set_palette(&game_palette);

    init_scene(&game_scene);
    scene_set_background(&game_scene, &background_image);

    init_sprite(&pointer_sprite, &pointer_class,
                world_to_screen_x((WORLD_MAX_X - WORLD_MIN_X) / 2),
                world_to_screen_y((WORLD_MAX_Y - WORLD_MIN_Y) / 2),
                0.0f, 0.0f, 0, 0, NULL);

    scene_add_sprite(&game_scene, &pointer_sprite);

    return 0;
}

void game_exit(void)
{
    destroy_scene(&game_scene);
    res_destroy_images();
}

void game_run(void)
{
    bool quit = false;

    float time = timer_get_time();
    float dt = time;

    while (!quit) {

        if (_kbhit() && getch() == KEY_ESC) {
            ungetch(KEY_ESC);
            quit = 1;
        }

        update_mouse();

        dt = timer_get_time_delta();
        time += dt;

        scene_update(&game_scene, time, dt);
        scene_draw(&game_scene);
        gfx_flip();
    }

    while (!_kbhit());
}
