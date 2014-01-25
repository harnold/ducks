#include "error.h"
#include <stdio.h>
#include <stdlib.h>

static void cleanup(void)
{
}

int main(void)
{
    error_set_log_file(stdout);

    atexit(cleanup);

    cleanup();

    log("Everything alright.\n");
    exit(EXIT_SUCCESS);

failure:

    cleanup();

    log("Something went wrong.\n");
    exit(EXIT_FAILURE);
}
