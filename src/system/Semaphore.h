#pragma once

#include "system_global.h"

#include <QSemaphore>

namespace paysages {
namespace system {

class Semaphore : private QSemaphore {
  public:
    Semaphore(int resources);

    inline void acquire() {
        QSemaphore::acquire();
    }
    inline void release() {
        QSemaphore::release();
    }
};
}
}
