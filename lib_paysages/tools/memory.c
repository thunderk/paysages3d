#include "memory.h"

#include <assert.h>

void* memory2dRealloc(void* data, int datasize, int oldxsize, int oldysize, int newxsize, int newysize, int xoffset, int yoffset)
{
    /* TODO Move remaining part*/
    return realloc(data, datasize * newxsize * newysize);
}

void memory2dScrolling(void* data, int datasize, int xsize, int ysize, int xoffset, int yoffset)
{
    /* TODO */
}
