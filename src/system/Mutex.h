#ifndef MUTEX_H
#define MUTEX_H

#include "system_global.h"
#include <QMutex>

namespace paysages {
namespace system {

/*!
 * \brief System mutex
 */
class SYSTEMSHARED_EXPORT Mutex : private QMutex {
  public:
    /*!
     * \brief Create a new mutex
     */
    Mutex();

    inline void acquire() {
        QMutex::lock();
    }
    inline void release() {
        QMutex::unlock();
    }
};
}
}

#endif // MUTEX_H
