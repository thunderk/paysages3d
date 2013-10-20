#ifndef _PAYSAGES_TOOLS_CACHE_H_
#define _PAYSAGES_TOOLS_CACHE_H_

/*
 * Cache management.
 */

#include "../rendering_global.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CacheFile CacheFile;

RENDERINGSHARED_EXPORT CacheFile* cacheFileCreateAccessor(const char* module, const char* ext, const char* tag1, int tag2, int tag3, int tag4, int tag5, int tag6);
RENDERINGSHARED_EXPORT void cacheFileDeleteAccessor(CacheFile* cache);
RENDERINGSHARED_EXPORT int cacheFileIsReadable(CacheFile* cache);
RENDERINGSHARED_EXPORT int cacheFileIsWritable(CacheFile* cache);
RENDERINGSHARED_EXPORT const char* cacheFileGetPath(CacheFile* cache);

#ifdef __cplusplus
}
#endif

#endif
