#ifndef LOGS_H
#define LOGS_H

#include "system_global.h"

#include <iostream>

namespace paysages {
namespace system {

/**
 * Logger streams
 */
class SYSTEMSHARED_EXPORT Logs {
  public:
    static ostream &debug();
    static ostream &warning();
    static ostream &error();

    // Log a timestamp on the debug output
    static void debugTimestamp(const string &message);

    // Disable all logs from now on
    static void disable();
};
}
}

#endif // LOGS_H
