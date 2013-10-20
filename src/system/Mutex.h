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
class Mutex: private QMutex
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

Mutex* mutexCreate();
void mutexDestroy(Mutex* mutex);
void mutexAcquire(Mutex* mutex);
void mutexRelease(Mutex* mutex);

#ifdef __cplusplus
}
#endif

#endif // MUTEX_H
