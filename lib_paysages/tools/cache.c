#include "cache.h"

#include <stdio.h>
#include <stdlib.h>
#include "../tools.h"

struct CacheFile
{
    char* filepath;
};

CacheFile* cacheFileCreateAccessor(const char* module, const char* ext, const char* tag1, int tag2, int tag3, int tag4, int tag5)
{
    CacheFile* result;

    result = (CacheFile*)malloc(sizeof(CacheFile));
    result->filepath = malloc(sizeof(char) * 501);

    snprintf(result->filepath, 500, "./cache/%s-%s-%d-%d-%d-%d.%s", module, tag1, tag2, tag3, tag4, tag5, ext);

    return result;
}

void cacheFileDeleteAccessor(CacheFile* cache)
{
    free(cache->filepath);
    free(cache);
}

int cacheFileIsReadable(CacheFile* cache)
{
    FILE* f = fopen(cache->filepath, "rb");
    if (f)
    {
        fclose(f);
        return 1;
    }
    else
    {
        return 0;
    }
}

int cacheFileIsWritable(CacheFile* cache)
{
    UNUSED(cache);

    FILE* f = fopen("./cache/.test", "wb");
    if (f)
    {
        fclose(f);
        return 1;
    }
    else
    {
        return 0;
    }
}

const char* cacheFileGetPath(CacheFile* cache)
{
    return cache->filepath;
}
