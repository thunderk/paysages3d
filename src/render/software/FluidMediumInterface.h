#ifndef FLUIDMEDIUMINTERFACE_H
#define FLUIDMEDIUMINTERFACE_H

#include "software_global.h"

namespace paysages {
namespace basics {
    class SpaceCoordinate;
}
namespace software {

/*!
 * \brief Interface to a fluid medium compatible class.
 */
class SOFTWARESHARED_EXPORT FluidMediumInterface
{
public:
    /*!
     * Return true if the object may interfere with the fluid medium on the given segment.
     */
    virtual bool mayInfluence(const SpaceCoordinate& start, const SpaceCoordinate& end) const = 0;
};

}
}

#endif // FLUIDMEDIUMINTERFACE_H
