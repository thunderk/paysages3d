#ifndef LOGS_H
#define LOGS_H

#include "system_global.h"

#include <iostream>

namespace paysages {
namespace system {

/**
 * Logger streams
 */
class Logs {
  public:
    static std::ostream &debug();
    static std::ostream &warning();
    static std::ostream &error();

    // Log a timestamp on the debug output
    static void debugTimestamp(const std::string &message);

    // Disable all logs from now on
    static void disable();
};
}
}

#endif // LOGS_H
