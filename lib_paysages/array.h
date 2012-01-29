#ifndef _PAYSAGES_ARRAY_H_
#define _PAYSAGES_ARRAY_H_

#include "shared/types.h"

#ifdef __cplusplus
extern "C" {
#endif

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
