#ifndef _PAYSAGES_TOOLS_ARRAY_H_
#define _PAYSAGES_TOOLS_ARRAY_H_

#include "../shared/types.h"
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**************** Naive methods (basic C arrays) ****************/

/*
 * Insert an item at a given position, rallocating the array, moving data and returning a pointer to the inserted element.
 */
void* naiveArrayInsert(void** array, size_t item_size, int item_count, int location);

/**************** Array object ****************/

typedef struct
{
    int length;
    int alloc_length;
    int item_size;
    int dirty;
    void* data;
} Array;

void arrayCreate(Array* array, int item_size);
void arrayDelete(Array* array);
void* arrayAppend(Array* array, void* item);
void arrayInsert(Array* array, void* item, int position);
void arrayReplace(Array* array, void* item, int position);
void arrayLStrip(Array* array, int count);
void arrayClear(Array* array);

#ifdef __cplusplus
}
#endif

#endif
