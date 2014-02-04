#ifndef DUCK_H
#define DUCK_H

#include "alloc.h"
#include "elist.h"
#include "sprite.h"

extern const struct sprite_class duck_classes[];

enum duck_state {
    DUCK_FLYING_LEFT,
    DUCK_FLYING_RIGHT,
    DUCK_FALLING_LEFT,
    DUCK_FALLING_RIGHT,
    DUCK_TRUDLING_LEFT,
    DUCK_TRUDLING_RIGHT
};

struct duck {
    int state;
    double world_x;
    double world_y;
    double world_v_x;
    double world_v_y;
    struct sprite *sprite;
    struct elist_node link;
};

DECLARE_ALLOCATOR(duck, struct duck);

#define duck_list_for_each(__duck, __list) \
    elist_for_each_elem((__duck), (__list), struct duck, link)

#define duck_list_get(__node) \
    elist_get(__node, struct duck, link)

void init_duck(struct duck *duck, int state, double x, double y,
               double v_x, double v_y, int z, float time);
void destroy_duck(struct duck *duck);
struct duck *create_duck(int state, double x, double y,
                         double v_x, double v_y, int z, float time);
void delete_duck(struct duck *duck);
void duck_cleanup(void);

#endif
