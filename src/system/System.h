#ifndef SYSTEM_H
#define SYSTEM_H

#include "system_global.h"

namespace paysages {
namespace system {

/**
 * Access to system info
 */
class SYSTEMSHARED_EXPORT System {
  public:
    /**
     * Get the number of CPU cores available on current system.
     */
    static int getCoreCount();

    /**
     * Get an identifying number for the current process.
     */
    static int getProcessId();
};
}
}

#endif // SYSTEM_H
