#include "Memory.h"

#include <string>
#include <cstring>
#include <cassert>

void *Memory::naiveArrayInsert(void **array, unsigned long item_size, int item_count, int location) {
    char **barray = (char **)array;

    assert(location >= 0);
    assert(location <= item_count);

    *barray = (char *)realloc(*barray, item_size * (item_count + 1));
    if (location < item_count) {
        memmove(*barray + item_size * (location + 1), *barray + item_size * location,
                item_size * (item_count - location));
    }

    return *barray + item_size * location;
}
