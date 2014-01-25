#include "error.h"
#include "timer.h"

#include <stdio.h>
#include <stdlib.h>

#define GAME_TIMER_HZ   120.0f

static void cleanup(void)
{
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

    cleanup();

    log("Everything alright.\n");
    exit(EXIT_SUCCESS);

failure:

    cleanup();

    log("Something went wrong.\n");
    exit(EXIT_FAILURE);
}
