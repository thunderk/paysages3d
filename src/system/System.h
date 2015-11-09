#ifndef SYSTEM_H
#define SYSTEM_H

#include "system_global.h"

namespace paysages {
namespace system {

/*!
 * \brief Access to system info
 */
class SYSTEMSHARED_EXPORT System {
  public:
    static int getCoreCount();
};
}
}

#endif // SYSTEM_H
