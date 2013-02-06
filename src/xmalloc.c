#include <errno.h>
#include <stdlib.h>

#include "error.h"
#include "xmalloc.h"

void *xmalloc(size_t size)
{
    if (size == 0)
        size = 1;

    void *ptr = malloc(size);

    if (!ptr)
        fatal_errno("Out of memory?  malloc() failed to allocate %ul bytes", size);

    return ptr;
}
