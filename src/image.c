#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "image.h"
#include "pcx.h"
#include "xmalloc.h"

struct image *create_image(int width, int height)
{
    struct image *image = xmalloc(sizeof(struct image));
    init_image(image, width, height, NULL);
    return image;
}

void delete_image(struct image *image)
{
    destroy_image(image);
    free(image);
}

void init_image(struct image *image, int width, int height, uint8_t *data)
{
    image->width = width;
    image->height = height;

    if (data != NULL)
        image->data = data;
    else
        image->data = xmalloc(width * height);
}

void destroy_image(struct image *image)
{
    free(image->data);
}

int load_image(const char *path, struct image *image)
{
    const char *ext = strrchr(path, '.');

    if (!ext) {
        error("File '%s' has no known image file format", path);
        return -1;
    }

    if (strcmp(ext, ".pcx") == 0) {
        return pcx_load_image(path, image);
    } else {
        error("File '%s' has no known image file format", path);
        return -1;
    }
}
