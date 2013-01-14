#ifndef _PAYSAGES_TOOLS_MEMORY_H_
#define _PAYSAGES_TOOLS_MEMORY_H_

/*
 * Memory tools.
 */

#ifdef __cplusplus
extern "C" {
#endif

void* memory2dRealloc(void* data, int datasize, int oldxsize, int oldysize, int newxsize, int newysize, int xoffset, int yoffset);
void memory2dScrolling(void* data, int datasize, int xsize, int ysize, int xoffset, int yoffset);

#ifdef __cplusplus
}
#endif

#endif
