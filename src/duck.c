#include "duck.h"
#include "res.h"
#include "world.h"

DEFINE_ALLOCATOR(duck, struct duck, ALLOC_DEFAULT_BLOB_SIZE);

static const struct animation duck_animations[] = {
    { ANIM_ROTATE_FORWARD, 60, 0, 79 },
    { ANIM_ROTATE_FORWARD, 60, 0, 79 },
    { ANIM_ROTATE_FORWARD, 60, 0, 79 },
    { ANIM_ROTATE_FORWARD, 60, 0, 79 }
};

const struct sprite_class duck_classes[] = {
    { &duck_flying_left_image, 100, 120, 80, 50.0f, 60.0f },
    { &duck_flying_right_image, 100, 120, 80, 50.0f, 60.0f },
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
}

void destroy_duck(struct duck *duck)
{
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
