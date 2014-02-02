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

#define KEY_ESC                 27

#define TIMER_DIGITS            2
#define TIMER_LAYER             0
#define TIMER_X_POS             32
#define TIMER_Y_POS             8

#define SCORE_DIGITS            5
#define SCORE_LAYER             0
#define SCORE_X_POS             608
#define SCORE_Y_POS             8

#define POINTER_LAYER           1

#define GAME_TIMEOUT            90

struct gfx_mode_info gfx_mode_info;

static const struct sprite_class pointer_class = {
    &pointer_image, 80, 80, 1, 40.0f, 40.0f
};

static const struct sprite_class digit_class = {
    &numbers_image, 32, 32, 1, 0.0f, 0.0f
};

static struct scene game_scene;
static struct sprite pointer_sprite;

static int timer;
static struct sprite timer_sprites[TIMER_DIGITS];

static int score;
static struct sprite score_sprites[SCORE_DIGITS];

static inline float confine_float(float x, float min, float max)
{
    return (x < min) ? min : (x > max) ? max : x;
}

static unsigned update_pointer(void)
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

static void update_number_display(struct sprite sprites[], int num_digits,
                                  unsigned int value)
{
    for (int i = num_digits - 1, shifter = 1; i >= 0; i--, shifter *= 10)
        sprites[i].frame = (value / shifter) % 10;
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
                world_to_screen_x(WORLD_SIZE_X / 2),
                world_to_screen_y(WORLD_SIZE_Y / 2),
                POINTER_LAYER, 0);

    scene_add_sprite(&game_scene, &pointer_sprite);

    timer = 0;

    for (int i = 0; i < TIMER_DIGITS; i++) {
        init_sprite(&timer_sprites[i], &digit_class,
                    world_to_screen_x(TIMER_X_POS + i * digit_class.width),
                    world_to_screen_y(TIMER_Y_POS),
                    TIMER_LAYER, 0);
        scene_add_sprite(&game_scene, &timer_sprites[i]);
    }

    score = 0;

    for (int i = 0; i < SCORE_DIGITS; i++) {
        init_sprite(&score_sprites[i], &digit_class,
                    world_to_screen_x(SCORE_X_POS + i * digit_class.width),
                    world_to_screen_y(SCORE_Y_POS),
                    SCORE_LAYER, 0);
        scene_add_sprite(&game_scene, &score_sprites[i]);
    }

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

    const float start_time = timer_get_time();
    float time = start_time;
    float elapsed_time = 0;

    while (!quit) {

        if (_kbhit() && getch() == KEY_ESC) {
            ungetch(KEY_ESC);
            quit = 1;
        }

        update_pointer();

        float dt = timer_get_time_delta();
        time += dt;
        elapsed_time += dt;

        if ((int) elapsed_time > GAME_TIMEOUT)
            break;

        update_number_display(timer_sprites, TIMER_DIGITS,
                              GAME_TIMEOUT - (int) elapsed_time);

        scene_update(&game_scene, time, dt);
        scene_draw(&game_scene);
        gfx_flip();
    }

    while (!_kbhit());
}
