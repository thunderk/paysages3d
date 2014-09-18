#ifndef CACHEFILE_H
#define CACHEFILE_H

#include "system_global.h"

namespace paysages {
namespace system {

class SYSTEMSHARED_EXPORT CacheFile
{
public:
    CacheFile(const std::string &module, const std::string &ext, const std::string &tag1, int tag2, int tag3, int tag4, int tag5, int tag6);

    bool isReadable();
    bool isWritable();
    std::string getPath();

private:
    std::string filepath;
};

}
}

#endif // CACHEFILE_H
