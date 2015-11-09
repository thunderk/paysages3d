#ifndef DATAFILE_H
#define DATAFILE_H

#include "system_global.h"

class QDir;

namespace paysages {
namespace system {

/**
 * Locator of data files.
 */
class SYSTEMSHARED_EXPORT DataFile {
  public:
    /**
     * Find a data file.
     *
     * Return the absolute data path, or an empty string if not found.
     */
    static std::string findFile(const std::string &relpath);

    /**
     * Find a data directory.
     *
     * Return the absolute data path, or an empty string if not found.
     */
    static std::string findDir(const std::string &relpath);

  private:
    static bool tryDataDir(const QDir &dir);
    static std::string locateDataDir();
    static std::string initDataDir();
    static std::string dataDir;
};
}
}

#endif // DATAFILE_H
