#include "error.h"
#include "game.h"
#include "gfx.h"
#include "mouse.h"
#include "timer.h"
#include "vbe.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define GAME_TIMER_HZ   120.0f

static void cleanup(void)
{
    game_exit();
    gfx_exit();
    timer_exit();
    mouse_exit();
}

int main(void)
{
    error_set_log_file(stdout);

    atexit(cleanup);

    srand((unsigned int) time(NULL));

    if (mouse_init() != 0) {
        error("Initializing mouse failed");
        goto failure;
    }

    if (timer_init(GAME_TIMER_HZ) != 0) {
        error("Initializing timer failed");
        goto failure;
    }

    if (gfx_init(VBE_MODE_800x600_256) != 0) {
        error("Initializing graphics failed");
        goto failure;
    }

    if (game_init() != 0) {
        error("Initializing game failed");
        goto failure;
    }

    int score = game_run();

    cleanup();

    printf("Congratulations! You reached a score of %d points!\n", score);

    exit(EXIT_SUCCESS);

failure:

    cleanup();

    exit(EXIT_FAILURE);
}
