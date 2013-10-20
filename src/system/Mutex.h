#ifndef MUTEX_H
#define MUTEX_H

#include "system_global.h"

#ifdef __cplusplus

#include <QMutex>

namespace paysages
{
namespace system
{

/*!
 * \brief System mutex
 */
class SYSTEMSHARED_EXPORT Mutex: private QMutex
{
public:
    /*!
     * \brief Create a new mutex
     */
    Mutex();

    inline void acquire() {QMutex::lock();}
    inline void release() {QMutex::unlock();}
};

}
}

extern "C" {
#endif

// Transitional C-API

#ifndef __cplusplus
typedef struct Mutex Mutex;
#endif

SYSTEMSHARED_EXPORT Mutex* mutexCreate();
SYSTEMSHARED_EXPORT void mutexDestroy(Mutex* mutex);
SYSTEMSHARED_EXPORT void mutexAcquire(Mutex* mutex);
SYSTEMSHARED_EXPORT void mutexRelease(Mutex* mutex);

#ifdef __cplusplus
}
#endif

#endif // MUTEX_H
