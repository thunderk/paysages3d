#ifndef _PAYSAGES_TOOLS_MEMORY_H_
#define _PAYSAGES_TOOLS_MEMORY_H_

/*
 * Memory tools.
 */

#include "../rendering_global.h"

#ifdef __cplusplus
extern "C" {
#endif

RENDERINGSHARED_EXPORT void* memory2dRealloc(void* data, int datasize, int oldxsize, int oldysize, int newxsize, int newysize, int xoffset, int yoffset);

#ifdef __cplusplus
}
#endif

#endif
