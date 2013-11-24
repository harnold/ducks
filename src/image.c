#include "image.h"
#include "error.h"
#include "pcx.h"
#include "xmalloc.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

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

    if (!ext)
        return error("File '%s' has no known image file format", path);

    if (strcmp(ext, ".pcx") == 0)
        return pcx_load_image(path, image);
    else
        return error("File '%s' has no known image file format", path);
}
