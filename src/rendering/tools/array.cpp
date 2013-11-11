#include "array.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

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

void arrayCreate(Array* array, int item_size)
{
    array->length = 0;
    array->alloc_length = 1;
    array->item_size = item_size;
    array->dirty = 1;
    array->data = malloc((size_t)item_size * array->alloc_length);
}

void arrayDelete(Array* array)
{
    free(array->data);
    array->data = NULL;
}

void* arrayAppend(Array* array, void* item)
{
    char* dest;
    size_t item_size = (size_t)array->item_size;

    if (array->length >= array->alloc_length)
    {
        array->alloc_length += 1;
        array->data = realloc(array->data, item_size * array->alloc_length);
    }

    dest = ((char*)array->data) + item_size * array->length;
    memcpy(dest, item, item_size);
    array->length++;

    array->dirty = 1;

    return dest;
}

void arrayInsert(Array* array, void* item, int position)
{
    size_t item_size;
    char* dest;

    if (position >= array->length)
    {
        arrayAppend(array, item);
    }
    else if (position >= 0)
    {
        item_size = (size_t)array->item_size;

        if (array->length >= array->alloc_length)
        {
            array->alloc_length += 10;
            array->data = realloc(array->data, item_size * array->alloc_length);
        }

        dest = ((char*)array->data) + item_size * position;
        memmove(dest + item_size, dest, array->length - position);
        memcpy(((char*)array->data) + item_size * position, item, item_size);
        array->length++;

        array->dirty = 1;
    }
}

void arrayReplace(Array* array, void* item, int position)
{
    size_t item_size;

    if (position >= 0 && position < array->length)
    {
        item_size = (size_t)array->item_size;
        memcpy(((char*)array->data) + item_size * position, item, item_size);

        array->dirty = 1;
    }
}

void arrayLStrip(Array* array, int count)
{
    size_t item_size;

    if (count >= array->length)
    {
        arrayClear(array);
    }
    else if (count >= 0)
    {
        item_size = (size_t)array->item_size;
        memmove(array->data, ((char*)array->data) + item_size * count, item_size * (array->length - count));
        array->length -= count;
        array->dirty = 1;
    }
}

void arrayClear(Array* array)
{
    free(array->data);
    array->length = 0;
    array->alloc_length = 1;
    array->data = malloc((size_t)array->item_size * array->alloc_length);
    array->dirty = 1;
}
