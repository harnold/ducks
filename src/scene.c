#include "scene.h"
#include "alloc.h"
#include "image.h"
#include "gfx.h"
#include "sprite.h"

void init_scene(struct scene *scene)
{
    scene->background = NULL;
    init_elist(&scene->sprite_list);
}

void destroy_scene(struct scene *scene)
{
}

void scene_set_background(struct scene *scene, struct image *image)
{
    scene->background = image;
}

void scene_add_sprite(struct scene *scene, struct sprite *sprite)
{
    struct elist_node *node = elist_begin(&scene->sprite_list);

    while (node != elist_end(&scene->sprite_list) &&
           sprite_list_get(node)->z >= sprite->z) {
        node = node->next;
    }

    elist_insert(&sprite->link, node);
}

void scene_remove_sprite(struct scene *scene, struct sprite *sprite)
{
    elist_remove(&sprite->link);
}

void scene_update(struct scene *scene, float t, float dt)
{
    struct sprite *sprite;

    sprite_list_for_each(sprite, &scene->sprite_list) {
        sprite_update(sprite, t, dt);
    }
}

void scene_draw(struct scene *scene)
{
    gfx_draw_image(scene->background, 0, 0, GFX_NO_CLIPPING);

    struct sprite *sprite;

    sprite_list_for_each(sprite, &scene->sprite_list) {
        gfx_draw_sprite(sprite, IMAGE_BLIT_MASK);
    }
}
