#ifndef MUTEX_H
#define MUTEX_H

#include "system_global.h"

#include <mutex>

namespace paysages {
namespace system {

/*!
 * \brief System mutex
 */
class SYSTEMSHARED_EXPORT Mutex : private mutex {
  public:
    /*!
     * \brief Create a new mutex
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

#endif // MUTEX_H
