#include "cache.h"

#include <stdio.h>
#include <stdlib.h>
#include "../tools.h"

struct CacheFile
{
    char* datapath;
    char* filepath;
};

CacheFile* cacheFileCreateAccessor(const char* module, const char* ext, const char* tag1, int tag2, int tag3, int tag4, int tag5, int tag6)
{
    CacheFile* result;

    result = (CacheFile*)malloc(sizeof(CacheFile));
    result->datapath = malloc(sizeof(char) * 501);
    result->filepath = malloc(sizeof(char) * 501);

    snprintf(result->datapath, 500, "/usr/share/paysages3d/%s-%s-%d-%d-%d-%d-%d.%s", module, tag1, tag2, tag3, tag4, tag5, tag6, ext);
    snprintf(result->filepath, 500, "./cache/%s-%s-%d-%d-%d-%d-%d.%s", module, tag1, tag2, tag3, tag4, tag5, tag6, ext);

    return result;
}

void cacheFileDeleteAccessor(CacheFile* cache)
{
    free(cache->datapath);
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
        FILE* f = fopen(cache->datapath, "rb");
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
    FILE* f = fopen(cache->filepath, "rb");
    if (f)
    {
        fclose(f);
        return cache->filepath;
    }
    else
    {
        return cache->datapath;
    }
}
