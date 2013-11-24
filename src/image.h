#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>

struct image {
    uint8_t *data;
    int width;
    int height;
};

void init_image(struct image *image, int width, int height, uint8_t *data);
void destroy_image(struct image *image);
int load_image(const char *path, struct image *image);

#endif
