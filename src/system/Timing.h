#pragma once

#include "system_global.h"

namespace paysages {
namespace system {

/**
 * Timing tools.
 */
class SYSTEMSHARED_EXPORT Timing {
  public:
    /**
     * Get a timestamp in milliseconds.
     */
    static unsigned long getRelativeTimeMs();
};
}
}
