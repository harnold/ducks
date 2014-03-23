#include "game.h"
#include "bits.h"
#include "compat.h"
#include "duck.h"
#include "elist.h"
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
#include <stdlib.h>

#define KEY_ESC                 27

#define TIMER_DIGITS            2
#define TIMER_LAYER             0
#define TIMER_X_POS             32
#define TIMER_Y_POS             8

#define SCORE_DIGITS            5
#define SCORE_LAYER             0
#define SCORE_X_POS             608
#define SCORE_Y_POS             8
#define SCORE_MIN_INC           10
#define SCORE_SCALE_FACTOR      10

#define POINTER_LAYER           1

#define GAME_TIMEOUT            90

#define DUCKS_LAYER             2
#define MAX_FLYING_DUCKS        20
#define MAX_ALL_DUCKS           40
#define DUCK_DENSITY            0.1f
#define DUCK_MIN_SPEED          (WORLD_SIZE_X / 10.0f)
#define DUCK_MAX_SPEED_1        (WORLD_SIZE_X / 1.0f)
#define DUCK_MAX_SPEED          (DUCK_MAX_SPEED_1 + DUCK_MIN_SPEED)
#define DUCK_MIN_HEIGHT         (0.7f * WORLD_SIZE_Y)
#define DUCK_FALLING_ACCEL      60.0f
#define DUCK_MIN_INTERVAL       2.0f
#define DUCK_MAX_INTERVAL_1     0.8f
#define DUCK_INTERVAL_SCALE     0.1f

struct gfx_mode_info gfx_mode_info;

static const struct sprite_class pointer_class = {
    &pointer_image, 80, 80, 1, 40.0f, 40.0f
};

static const struct sprite_class digit_class = {
    &numbers_image, 32, 32, 10, 0.0f, 0.0f
};

static struct scene game_scene;
static struct sprite pointer_sprite;
static struct sprite timer_sprites[TIMER_DIGITS];
static struct sprite score_sprites[SCORE_DIGITS];

static int game_score;
static float elapsed_time;
static int num_flying_ducks;
static int num_falling_ducks;
static struct elist flying_ducks_list;
static struct elist falling_ducks_list;
static float time_of_next_duck;

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

static void create_ducks(float time)
{
    if (num_flying_ducks >= MAX_FLYING_DUCKS ||
        num_flying_ducks + num_falling_ducks >= MAX_ALL_DUCKS ||
        time < time_of_next_duck)
        return;

    int state = (frand() > 0.5) ? DUCK_FLYING_LEFT : DUCK_FLYING_RIGHT;
    const struct sprite_class *class = &duck_classes[state];

    float off_x = screen_to_world_dx(class->origin_x);
    float off_y = screen_to_world_dy(class->origin_y);

    float world_x;
    float world_y = frand() * DUCK_MIN_HEIGHT + off_y;
    float world_v_x = frand() * DUCK_MAX_SPEED_1 + DUCK_MIN_SPEED;

    if (state == DUCK_FLYING_LEFT) {
        world_x = WORLD_MAX_X + off_x;
        world_v_x = -world_v_x;
    } else {
        world_x = WORLD_MIN_X - off_x;
    }

    struct duck *duck = create_duck(state, world_x, world_y, world_v_x, 0.0f,
                                    DUCKS_LAYER, time);

    elist_insert_back(&duck->link, &flying_ducks_list);
    scene_add_sprite(&game_scene, duck->sprite);
    num_flying_ducks++;

    float time_term = 1.0f - elapsed_time / GAME_TIMEOUT;
    float duck_term = 1 + num_flying_ducks;

    time_of_next_duck +=
        time_term * time_term * DUCK_MIN_INTERVAL +
        duck_term * frand() * DUCK_INTERVAL_SCALE * DUCK_MAX_INTERVAL_1;
}

static void update_flying_ducks(float dt)
{
    struct elist_node *node, *tmp;

    elist_for_each_node_safe(node, tmp, &flying_ducks_list) {

        struct duck *duck = duck_list_get(node);

        duck->world_x += dt * duck->world_v_x;
        duck->world_y += dt * duck->world_v_y;

        duck->sprite->x = world_to_screen_x(duck->world_x);
        duck->sprite->y = world_to_screen_y(duck->world_y);

        if (!duck_visible(duck)) {
            delete_duck(duck);
            num_flying_ducks--;
        }
    }
}

static void update_falling_ducks(float time, float dt)
{
    struct elist_node *node, *tmp;

    elist_for_each_node_safe(node, tmp, &falling_ducks_list) {

        struct duck *duck = duck_list_get(node);

        if (duck->state == DUCK_FALLING_LEFT ||
            duck->state == DUCK_FALLING_RIGHT) {

            float truddling_start_time =
                duck->sprite->anim_start_time + duck_seconds_falling;

            if (time >= truddling_start_time) {

                int new_state = (duck->state == DUCK_FALLING_LEFT) ?
                    DUCK_TRUDLING_LEFT :
                    DUCK_TRUDLING_RIGHT;

                duck_set_state(duck, new_state, truddling_start_time);
            }
        }

        duck->world_v_y += DUCK_FALLING_ACCEL * dt;
        duck->world_x += dt * duck->world_v_x;
        duck->world_y += dt * duck->world_v_y;

        duck->sprite->x = world_to_screen_x(duck->world_x);
        duck->sprite->y = world_to_screen_y(duck->world_y);

        if (!duck_visible(duck)) {
            delete_duck(duck);
            num_falling_ducks--;
        }
    }
}

static void shoot_and_test_hit(int x, int y, float time)
{
    struct elist_node *node, *tmp;

    elist_for_each_node_safe(node, tmp, &flying_ducks_list) {

        struct duck *duck = duck_list_get(node);

        if (!duck_test_hit(duck, x, y))
            continue;

        float speed_fact = (duck->world_v_x - DUCK_MIN_SPEED) / DUCK_MAX_SPEED;
        game_score += (int) (SCORE_SCALE_FACTOR * (speed_fact + 1.0f)) * SCORE_MIN_INC;

        update_number_display(score_sprites, SCORE_DIGITS, game_score);

        int new_state = (duck->state == DUCK_FLYING_LEFT) ?
            DUCK_FALLING_LEFT :
            DUCK_FALLING_RIGHT;

        duck_set_state(duck, new_state, time);
        elist_move(node, elist_end(&falling_ducks_list));
        num_flying_ducks--;
        num_falling_ducks++;

        return;
    }
}

static void destroy_ducks(void)
{
    struct elist_node *node, *tmp;

    elist_for_each_node_safe(node, tmp, &flying_ducks_list)
        delete_duck(duck_list_get(node));

    elist_for_each_node_safe(node, tmp, &falling_ducks_list)
        delete_duck(duck_list_get(node));
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

    for (int i = 0; i < TIMER_DIGITS; i++) {
        init_sprite(&timer_sprites[i], &digit_class,
                    world_to_screen_x(TIMER_X_POS) + i * digit_class.width,
                    world_to_screen_y(TIMER_Y_POS),
                    TIMER_LAYER, 0);
        scene_add_sprite(&game_scene, &timer_sprites[i]);
    }

    game_score = 0;

    for (int i = 0; i < SCORE_DIGITS; i++) {
        init_sprite(&score_sprites[i], &digit_class,
                    world_to_screen_x(SCORE_X_POS) + i * digit_class.width,
                    world_to_screen_y(SCORE_Y_POS),
                    SCORE_LAYER, 0);
        scene_add_sprite(&game_scene, &score_sprites[i]);
    }

    num_flying_ducks = 0;
    num_falling_ducks = 0;
    init_elist(&flying_ducks_list);
    init_elist(&falling_ducks_list);
    time_of_next_duck = 0.0f;

    return 0;
}

void game_exit(void)
{
    destroy_ducks();
    destroy_scene(&game_scene);
    res_destroy_images();

    duck_cleanup();
    sprite_cleanup();
    scene_cleanup();
}

int game_run(void)
{
    bool quit = false;

    const float start_time = timer_get_time();
    float time = start_time;
    elapsed_time = 0;

    unsigned mouse_button_old = MOUSE_NO_BUTTON;

    while (!quit) {

        if (_kbhit() && getch() == KEY_ESC) {
            ungetch(KEY_ESC);
            quit = true;
        }

        float dt = timer_get_time_delta();
        time += dt;
        elapsed_time += dt;

        if ((int) elapsed_time > GAME_TIMEOUT)
            break;

        update_number_display(timer_sprites, TIMER_DIGITS,
                              GAME_TIMEOUT - (int) elapsed_time);

        unsigned mouse_button = update_pointer();

        if (test_bit(mouse_button, MOUSE_LEFT_BUTTON) &&
            !test_bit(mouse_button_old, MOUSE_LEFT_BUTTON)) {

            shoot_and_test_hit(pointer_sprite.x, pointer_sprite.y, time);
        }

        mouse_button_old = mouse_button;

        create_ducks(time);
        update_flying_ducks(dt);
        update_falling_ducks(time, dt);

        scene_update(&game_scene, time, dt);
        scene_draw(&game_scene);
        gfx_flip();
    }

    while (!_kbhit());
    getch();

    return game_score;
}
