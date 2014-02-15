#include "duck.h"
#include "res.h"
#include "world.h"

DEFINE_ALLOCATOR(duck, struct duck, ALLOC_DEFAULT_BLOB_SIZE);

#define DUCK_HIT_RADIUS_X       3.5f
#define DUCK_HIT_RADIUS_Y       4.0f

const struct animation duck_animations[] = {
    { ANIM_ROTATE_FORWARD, 60, 0, 79 },
    { ANIM_ROTATE_FORWARD, 60, 0, 79 },
    { ANIM_ONCE_FORWARD, 60, 0, 39 },
    { ANIM_ONCE_FORWARD, 60, 0, 39 },
    { ANIM_ROTATE_FORWARD, 60, 40, 79 },
    { ANIM_ROTATE_FORWARD, 60, 40, 79 },
};

const float duck_seconds_falling = 40 / 60.0f;

const struct sprite_class duck_classes[] = {
    { &duck_flying_left_image, 100, 120, 80, 50.0f, 60.0f },
    { &duck_flying_right_image, 100, 120, 80, 50.0f, 60.0f },
    { &duck_falling_left_image, 160, 120, 80, 50.0f, 60.0f },
    { &duck_falling_right_image, 160, 120, 80, 50.0f, 60.0f },
    { &duck_falling_left_image, 160, 120, 80, 50.0f, 60.0f },
    { &duck_falling_right_image, 160, 120, 80, 50.0f, 60.0f }
};

void init_duck(struct duck *duck, int state, double x, double y,
               double v_x, double v_y, int z, float time)
{
    duck->state = state;
    duck->world_x = x;
    duck->world_y = y;
    duck->world_v_x = v_x;
    duck->world_v_y = v_y;

    struct sprite *sprite = create_sprite(&duck_classes[state],
                                          world_to_screen_x(x),
                                          world_to_screen_y(y), z, 0);

    sprite_set_animation(sprite, &duck_animations[state], time);

    duck->sprite = sprite;
    elist_link(&duck->link, &duck->link);
}

void destroy_duck(struct duck *duck)
{
    elist_remove(&duck->link);
    delete_sprite(duck->sprite);
}

struct duck *create_duck(int state, double x, double y,
                         double v_x, double v_y, int z, float time)
{
    struct duck *duck = alloc_duck();
    init_duck(duck, state, x, y, v_x, v_y, z, time);
    return duck;
}

void delete_duck(struct duck *duck)
{
    destroy_duck(duck);
    free_duck(duck);
}

void duck_set_state(struct duck *duck, int state, float time)
{
    duck->state = state;
    const struct sprite_class *class = &duck_classes[state];
    *((struct sprite_class *) duck->sprite) = *class;
    sprite_set_animation(duck->sprite, &duck_animations[state], time);
}

void duck_cleanup(void)
{
    free_all_duck_allocs();
}

bool duck_test_hit(struct duck *duck, int screen_x, int screen_y)
{
    int duck_x = duck->sprite->x;
    int duck_y = duck->sprite->y;

    int dx = (int) (duck->sprite->width / DUCK_HIT_RADIUS_X);
    int dy = (int) (duck->sprite->height / DUCK_HIT_RADIUS_Y);

    return screen_x >= duck_x - dx && screen_x <= duck_x + dx
        && screen_y >= duck_y - dy && screen_y <= duck_y + dy;
}

bool duck_visible(struct duck *duck)
{
    struct sprite *sprite = duck->sprite;

    if (sprite->y > gfx_mode_info.y_resolution + sprite->origin_y)
        return false;

    switch (duck->state) {

    case DUCK_FLYING_LEFT:
    case DUCK_FALLING_LEFT:
    case DUCK_TRUDLING_LEFT:

        return sprite->x >= -sprite->origin_x;

    default:

        return sprite->x <= gfx_mode_info.x_resolution + sprite->origin_x;
    }
}
