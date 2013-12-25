#include "CacheFile.h"

CacheFile::CacheFile(const char* module, const char* ext, const char* tag1, int tag2, int tag3, int tag4, int tag5, int tag6)
{
    datapath = (char*)malloc(sizeof(char) * 501);
    filepath = (char*)malloc(sizeof(char) * 501);

    snprintf(datapath, 500, "./data/cache/%s-%s-%d-%d-%d-%d-%d.%s", module, tag1, tag2, tag3, tag4, tag5, tag6, ext);
    snprintf(filepath, 500, "./cache/%s-%s-%d-%d-%d-%d-%d.%s", module, tag1, tag2, tag3, tag4, tag5, tag6, ext);
}

CacheFile::~CacheFile()
{
    free(datapath);
    free(filepath);
}

bool CacheFile::isReadable()
{
    FILE* f = fopen(filepath, "rb");
    if (f)
    {
        fclose(f);
        return true;
    }
    else
    {
        FILE* f = fopen(datapath, "rb");
        if (f)
        {
            fclose(f);
            return true;
        }
        else
        {
            return false;
        }
    }
}

bool CacheFile::isWritable()
{
    FILE* f = fopen("./cache/.test", "wb");
    if (f)
    {
        fclose(f);
        return true;
    }
    else
    {
        return false;
    }
}

const char* CacheFile::getPath()
{
    FILE* f = fopen(datapath, "rb");
    if (f)
    {
        fclose(f);
        return datapath;
    }
    else
    {
        return filepath;
    }
}
