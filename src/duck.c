#include "duck.h"
#include "res.h"
#include "sprite.h"
#include "world.h"

DEFINE_ALLOCATOR(duck, struct duck, ALLOC_DEFAULT_BLOB_SIZE);

static const struct sprite_class duck_classes[] = {
    { &duck_flying_left_image, 100, 120, 80, 50, 60 },
    { &duck_flying_right_image, 100, 120, 80, 50, 60 },
    { &duck_falling_left_image, 160, 120, 80, 50, 60 },
    { &duck_falling_right_image, 160, 120, 80, 50, 60 }
};

static const struct animation duck_animations[] = {
    { ANIM_ROTATE_FORWARD, 60, 0, 79 },
    { ANIM_ROTATE_FORWARD, 60, 0, 79 },
    { ANIM_ROTATE_FORWARD, 60, 0, 79 },
    { ANIM_ROTATE_FORWARD, 60, 0, 79 }
};

void init_duck(struct duck *duck, int state, double x, double y,
               double v_x, double v_y, int z)
{
    duck->state = state;
    duck->world_x = x;
    duck->world_y = y;
    duck->world_v_x = v_x;
    duck->world_v_y = v_y;

    struct sprite *sprite = alloc_sprite();

    init_sprite(sprite, &duck_classes[state],
                world_to_screen_x(x), world_to_screen_y(y),
                world_to_screen_dx(v_x), world_to_screen_dy(v_y),
                z, 0, &duck_animations[state]);

    duck->sprite = sprite;
}

void destroy_duck(struct duck *duck)
{
    free_sprite(duck->sprite);
}
