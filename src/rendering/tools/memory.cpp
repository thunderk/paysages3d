#include "memory.h"

#include <cstdlib>
#include <cstring>
#include <cassert>

void* naiveArrayInsert(void** array, size_t item_size, int item_count, int location)
{
    char** barray = (char**)array;

    assert(location >= 0);
    assert(location <= item_count);

    *barray = (char*)realloc(*barray, item_size * (item_count + 1));
    if (location < item_count)
    {
        memmove(*barray + item_size * (location + 1), *barray + item_size * location, item_size * (item_count - location));
    }

    return *barray + item_size * location;
}

char* memory2dRealloc(char* data, int datasize, int oldxsize, int oldysize, int newxsize, int newysize, int xoffset, int yoffset)
{
    int xstart, xend, xlen;
    int ystart, yend, y;
    char* result = (char*)malloc(datasize * newxsize * newysize);

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
