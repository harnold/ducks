#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>

struct palette;

struct image {
    uint8_t *data;
    int width;
    int height;
};

struct image *create_image(int width, int height);
void delete_image(struct image *image);
void init_image(struct image *image, int width, int height, uint8_t *data);
void destroy_image(struct image *image);
int load_palette(const char *path, struct palette *palette);
int load_image(const char *path, struct image *image);

#endif
