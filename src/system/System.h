#pragma once

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

#ifdef PLATFORM_WIN
#define TMP_DIRECTORY "c:/temp/"
#else
#define TMP_DIRECTORY "/tmp/"
#endif
