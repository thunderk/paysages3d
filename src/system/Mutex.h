#pragma once

#include "system_global.h"

#include <mutex>

namespace paysages {
namespace system {

/**
 * System mutex
 */
class SYSTEMSHARED_EXPORT Mutex : private mutex {
  public:
    /**
     * Create a new mutex
     */
    Mutex();

    inline void acquire() {
        mutex::lock();
    }
    inline void release() {
        mutex::unlock();
    }
};
}
}
