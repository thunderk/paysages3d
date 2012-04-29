#include "array.h"

#include <stdlib.h>
#include <string.h>

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
    void* dest;
    size_t item_size = (size_t)array->item_size;

    if (array->length >= array->alloc_length)
    {
        array->alloc_length += 1;
        array->data = realloc(array->data, item_size * array->alloc_length);
    }

    dest = array->data + item_size * array->length;
    memcpy(dest, item, item_size);
    array->length++;

    array->dirty = 1;

    return dest;
}

void arrayInsert(Array* array, void* item, int position)
{
    size_t item_size;
    void* dest;

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

        dest = array->data + item_size * position;
        memmove(dest + item_size, dest, array->length - position);
        memcpy(array->data + item_size * position, item, item_size);
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
        memcpy(array->data + item_size * position, item, item_size);

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
        memmove(array->data, array->data + item_size * count, item_size * (array->length - count));
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
