#include "game.h"
#include "res.h"

#include <conio.h>
#include <stdbool.h>

#define KEY_ESC         27


int game_init(void)
{
    if (res_load_palettes() != 0)
        return -1;

    if (res_load_images() != 0) {
        res_destroy_images();
        return -1;
    }

    return 0;
}

void game_exit(void)
{
    res_destroy_images();
}

void game_run(void)
{
    bool quit = false;

    while (!quit) {

        if (_kbhit() && getch() == KEY_ESC) {
            ungetch(KEY_ESC);
            quit = 1;
        }

    }

    while (!_kbhit());
}
