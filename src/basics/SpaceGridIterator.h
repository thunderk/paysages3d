#pragma once

#include "basics_global.h"

namespace paysages {
namespace basics {

/**
 * Iterator delegate that can receive sequential grid coordinates.
 *
 * This may be useful for ray marching algorithms for example.
 */
class BASICSSHARED_EXPORT SpaceGridIterator {
  public:
    SpaceGridIterator();

    /**
     * Abstract method to implement to receive grid coordinates.
     *
     * Return false to interrupt the iteration, true to continue.
     */
    virtual bool onCell(int x, int y, int z) = 0;
};
}
}
