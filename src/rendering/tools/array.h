#ifndef _PAYSAGES_TOOLS_ARRAY_H_
#define _PAYSAGES_TOOLS_ARRAY_H_

#include "../rendering_global.h"
#include "../shared/types.h"
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**************** Naive methods (basic C arrays) ****************/

/*
 * Insert an item at a given position, rallocating the array, moving data and returning a pointer to the inserted element.
 */
RENDERINGSHARED_EXPORT void* naiveArrayInsert(void** array, size_t item_size, int item_count, int location);

/**************** Array object ****************/

typedef struct
{
    int length;
    int alloc_length;
    int item_size;
    int dirty;
    void* data;
} Array;

RENDERINGSHARED_EXPORT void arrayCreate(Array* array, int item_size);
RENDERINGSHARED_EXPORT void arrayDelete(Array* array);
RENDERINGSHARED_EXPORT void* arrayAppend(Array* array, void* item);
RENDERINGSHARED_EXPORT void arrayInsert(Array* array, void* item, int position);
RENDERINGSHARED_EXPORT void arrayReplace(Array* array, void* item, int position);
RENDERINGSHARED_EXPORT void arrayLStrip(Array* array, int count);
RENDERINGSHARED_EXPORT void arrayClear(Array* array);

#ifdef __cplusplus
}
#endif

#endif
