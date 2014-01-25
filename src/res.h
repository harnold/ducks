#ifndef RES_H
#define RES_H

#include "image.h"
#include "palette.h"

extern struct palette game_palette;
extern struct image background_image;
extern struct image duck_flying_left_image;
extern struct image duck_flying_right_image;
extern struct image duck_falling_left_image;
extern struct image duck_falling_right_image;
extern struct image pointer_image;
extern struct image numbers_image;

int res_load_palettes(void);
int res_load_images(void);
void res_destroy_images(void);

#endif
