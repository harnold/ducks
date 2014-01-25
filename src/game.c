#include "game.h"
#include "gfx.h"
#include "image.h"
#include "palette.h"
#include "res.h"
#include "scene.h"
#include "timer.h"
#include "vga.h"

#include <conio.h>
#include <stdbool.h>

#define KEY_ESC         27

struct gfx_mode_info gfx_mode_info;

static struct scene game_scene;

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

        dt = timer_get_time_delta();
        time += dt;

        scene_update(&game_scene, time, dt);
        scene_draw(&game_scene);
        gfx_flip();
    }

    while (!_kbhit());
}
