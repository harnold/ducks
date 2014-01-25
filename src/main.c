#include "error.h"
#include "gfx.h"
#include "timer.h"
#include "vbe.h"

#include <stdio.h>
#include <stdlib.h>

#define GAME_TIMER_HZ   120.0f

static void cleanup(void)
{
    gfx_exit();
    timer_exit();
}

int main(void)
{
    error_set_log_file(stdout);

    atexit(cleanup);

    if (timer_init(GAME_TIMER_HZ) != 0) {
        error("Initializing timer failed");
        goto failure;
    }

    if (gfx_init(VBE_MODE_800x600_256) != 0) {
        error("Initializing graphics failed");
        goto failure;
    }

    cleanup();

    log("Everything alright.\n");
    exit(EXIT_SUCCESS);

failure:

    cleanup();

    log("Something went wrong.\n");
    exit(EXIT_FAILURE);
}
