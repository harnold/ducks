#ifndef SPRITE_H
#define SPRITE_H

#include "alloc.h"
#include "elist.h"

#include <stdint.h>

struct image;

enum animation_type {
    ANIM_ROTATE_FORWARD,
    ANIM_ROTATE_BACKWARD,
    ANIM_ONCE_FORWARD,
    ANIM_ONCE_BACKWARD
};

struct animation {
    enum animation_type type;
    float fps;
    int start_frame;
    int end_frame;
};

struct sprite_class {
    struct image *image;
    int width;
    int height;
    int num_frames;
    float origin_x;
    float origin_y;
};

struct sprite {
    struct image *image;
    int width;
    int height;
    int num_frames;
    float origin_x;
    float origin_y;
    float x;
    float y;
    float v_x;
    float v_y;
    int z;
    int frame;
    const struct animation *anim;
    float anim_start_time;
    struct elist_node link;
};

DECLARE_ALLOCATOR(sprite, struct sprite);

#define sprite_list_for_each(__sprite, __list) \
    elist_for_each_elem((__sprite), (__list), struct sprite, link)

#define sprite_list_get(__node) \
    elist_get(__node, struct sprite, link)

void init_sprite_class(struct sprite_class *class, struct image *image,
                       int width, int height, int num_frames,
                       float origin_x, float origin_y);
void init_sprite(struct sprite *sprite, const struct sprite_class *class,
                 float x, float y, float v_x, float v_y, int z, int frame,
                 const struct animation *anim);
void sprite_update(struct sprite *sprite, float t, float dt);

#endif
