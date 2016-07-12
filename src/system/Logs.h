#pragma once

#include "system_global.h"

#include <iostream>

namespace paysages {
namespace system {

/**
 * Logger streams
 */
class SYSTEMSHARED_EXPORT Logs {
  public:
    static ostream &debug(const string &logger);
    static ostream &warning(const string &logger);
    static ostream &error(const string &logger);

    // Log a timestamp on the debug output
    static void debugTimestamp(const string &logger, const string &message);

    // Disable all logs from now on
    static void disable();
};
}
}
