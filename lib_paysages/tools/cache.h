#ifndef _PAYSAGES_TOOLS_CACHE_H_
#define _PAYSAGES_TOOLS_CACHE_H_

/*
 * Cache management.
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CacheFile CacheFile;

CacheFile* cacheFileCreateAccessor(const char* module, const char* ext, const char* tag1, int tag2, int tag3, int tag4, int tag5, int tag6);
void cacheFileDeleteAccessor(CacheFile* cache);
int cacheFileIsReadable(CacheFile* cache);
int cacheFileIsWritable(CacheFile* cache);
const char* cacheFileGetPath(CacheFile* cache);

#ifdef __cplusplus
}
#endif

#endif
