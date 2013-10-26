#ifndef SYSTEM_H
#define SYSTEM_H

#include "system_global.h"

#ifdef __cplusplus

namespace paysages
{
namespace system
{

/*!
 * \brief Access to system info
 */
class SYSTEMSHARED_EXPORT System
{
public:
    static int getCoreCount();
};

}
}

extern "C" {
#endif

// Transitional C-API
int systemGetCoreCount();

#ifdef __cplusplus
}
#endif

#endif // SYSTEM_H
