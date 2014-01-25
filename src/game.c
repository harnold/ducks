#include "game.h"

#include <conio.h>
#include <stdbool.h>

#define KEY_ESC         27


int game_init(void)
{
    return 0;
}

void game_exit(void)
{
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
