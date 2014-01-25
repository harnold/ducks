#include "res.h"
#include "error.h"

#include <stddef.h>

struct palette game_palette;

struct image background_image;
struct image duck_flying_left_image;
struct image duck_flying_right_image;
struct image duck_falling_left_image;
struct image duck_falling_right_image;
struct image pointer_image;
struct image numbers_image;

struct palette_info {
    struct palette *palette;
    const char *path;
};

struct image_info {
    struct image *image;
    const char *path;
};

static const struct palette_info palette_infos[] = {
    &game_palette, "gfx\\sky.pcx",
    NULL, NULL
};

static const struct image_info image_infos[] = {
    &background_image, "gfx\\sky.pcx",
    &duck_flying_left_image, "gfx\\fly_l.pcx",
    &duck_flying_right_image, "gfx\\fly_r.pcx",
    &duck_falling_left_image, "gfx\\fall_l.pcx",
    &duck_falling_right_image, "gfx\\fall_r.pcx",
    &pointer_image, "gfx\\pointer.pcx",
    &numbers_image, "gfx\\numbers.pcx",
    NULL, NULL
};

int res_load_palettes(void)
{
    const struct palette_info *info;

    for (info = palette_infos; info->palette != NULL; info++) {
        if (load_palette(info->path, info->palette) != 0)
            return error("Loading palette from file %s failed", info->path);
    }

    return 0;
}

int res_load_images(void)
{
    const struct image_info *info;

    for (info = image_infos; info->image != NULL; info++) {
        if (load_image(info->path, info->image) != 0)
            return error("Loading image from file %s failed", info->path);
    }

    return 0;
}

void res_destroy_images(void)
{
    const struct image_info *info;

    for (info = image_infos; info->image != NULL; info++) {
        if (info->image->data != NULL) {
            destroy_image(info->image);
            info->image->data = NULL;
        }
    }
}
