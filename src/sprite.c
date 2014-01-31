#include "sprite.h"

#include <stdlib.h>

DEFINE_ALLOCATOR(sprite, struct sprite, ALLOC_DEFAULT_BLOB_SIZE);

void init_sprite_class(struct sprite_class *class, struct image *image,
                      int width, int height, int num_frames,
                      float origin_x, float origin_y)
{
    class->image = image;
    class->width = width;
    class->height = height;
    class->num_frames = num_frames;
    class->origin_x = origin_x;
    class->origin_y = origin_y;
}

void init_sprite(struct sprite *sprite, const struct sprite_class *class,
                 float x, float y, float v_x, float v_y, int z, int frame,
                 const struct animation *anim)
{
    *((struct sprite_class *) sprite) = *class;
    sprite->x = x;
    sprite->y = y;
    sprite->v_x = v_x;
    sprite->v_y = v_y;
    sprite->z = z;
    sprite->frame = frame;
    sprite->anim = anim;
    sprite->anim_start_time = 0.0f;
}

void sprite_update(struct sprite *sprite, float t, float dt)
{
    sprite->x += dt * sprite->v_x;
    sprite->y += dt * sprite->v_y;

    const struct animation *anim = sprite->anim;

    if (anim == NULL)
        return;

    int nframe = (int) ((t - sprite->anim_start_time) * anim->fps);

    switch (anim->type) {

    case ANIM_ROTATE_FORWARD:
        sprite->frame = anim->start_frame +
            nframe % (anim->end_frame - anim->start_frame + 1);
        break;

    case ANIM_ROTATE_BACKWARD:
        sprite->frame = anim->start_frame -
            nframe % (anim->start_frame - anim->end_frame + 1);
        break;

    case ANIM_ONCE_FORWARD:
        if (sprite->frame < anim->end_frame)
            sprite->frame = min(anim->start_frame + nframe, anim->end_frame);
        break;

    case ANIM_ONCE_BACKWARD:
        if (sprite->frame > anim->end_frame)
            sprite->frame = max(anim->start_frame - nframe, anim->end_frame);
        break;
    }
}
