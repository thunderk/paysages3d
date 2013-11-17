#ifndef _PAYSAGES_TOOLS_MEMORY_H_
#define _PAYSAGES_TOOLS_MEMORY_H_

/*
 * Memory tools.
 */

#include "../rendering_global.h"

RENDERINGSHARED_EXPORT char* memory2dRealloc(char* data, int datasize, int oldxsize, int oldysize, int newxsize, int newysize, int xoffset, int yoffset);

#endif
