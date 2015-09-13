#ifndef TIME_H
#define TIME_H

#include "system_global.h"

namespace paysages {
namespace system {

class SYSTEMSHARED_EXPORT Time
{
public:
    /**
     * Get a timestamp in milliseconds.
     */
    static unsigned long getRelativeTimeMs();
};

}
}

#endif // TIME_H
