#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "system_global.h"

namespace paysages {
namespace system {

class SYSTEMSHARED_EXPORT FileSystem {
  public:
    /**
     * Get an absolute path to a temporary file.
     *
     * filename must not contain directory separators.
     */
    static string getTempFile(const string &filename);

    /**
     * Returns true if the given path points to a file.
     */
    static bool isFile(const string &filepath);

    /**
     * Remove a file by its absolute path.
     */
    static bool removeFile(const string &filepath);
};
}
}

#endif // FILESYSTEM_H
