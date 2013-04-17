#include "memory.h"

#include <stdlib.h>
#include <string.h>

void* memory2dRealloc(void* data, int datasize, int oldxsize, int oldysize, int newxsize, int newysize, int xoffset, int yoffset)
{
    int xstart, xend, xlen;
    int ystart, yend, y;
    void* result = malloc(datasize * newxsize * newysize);

    /* Move remaining part*/
    ystart = yoffset;
    yend = yoffset + oldysize;
    if (yend >= 0 && ystart < newysize)
    {
        if (ystart < 0)
        {
            ystart = 0;
        }
        if (yend > newysize - 1)
        {
            yend = newysize - 1;
        }

        xstart = xoffset;
        xend = xoffset + oldxsize;
        if (xend >= 0 && xstart < newxsize)
        {
            if (xstart < 0)
            {
                xstart = 0;
            }
            if (xend > newxsize - 1)
            {
                xend = newxsize - 1;
            }

            xlen = xend - xstart + 1;
            if (xlen > 0)
            {
                for (y = ystart; y <= yend; y++)
                {
                    memcpy(result + (y * newxsize + xstart) * datasize, data + ((y - ystart) * oldxsize) * datasize, xlen * datasize);
                }
            }
        }
    }

    free(data);
    return result;
}

void memory2dScrolling(void* data, int datasize, int xsize, int ysize, int xoffset, int yoffset)
{
    /* TODO */
}
