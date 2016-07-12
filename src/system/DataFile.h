#pragma once

#include "system_global.h"

#include <string>

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
    static string findFile(const string &relpath);

    /**
     * Find a data directory.
     *
     * Return the absolute data path, or an empty string if not found.
     */
    static string findDir(const string &relpath);

  private:
    static bool tryDataDir(const QDir &dir);
    static string locateDataDir();
    static string initDataDir();
    static string dataDir;
};
}
}
