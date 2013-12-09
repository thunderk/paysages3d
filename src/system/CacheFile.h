#ifndef CACHEFILE_H
#define CACHEFILE_H

#include "system_global.h"

namespace paysages {
namespace system {

class SYSTEMSHARED_EXPORT CacheFile
{
public:
    CacheFile(const char* module, const char* ext, const char* tag1, int tag2, int tag3, int tag4, int tag5, int tag6);
    ~CacheFile();

    bool isReadable();
    bool isWritable();
    const char* getPath();

private:
    char* datapath;
    char* filepath;
};

}
}

#endif // CACHEFILE_H
