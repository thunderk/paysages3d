#ifndef FLUIDMEDIUMTRAVERSAL_H
#define FLUIDMEDIUMTRAVERSAL_H

#include "software_global.h"

#include <vector>
#include "SpaceCoordinates.h"

namespace paysages {
namespace software {

class SoftwareRenderer;
class FluidMediumCollector;
class FluidMediumInterface;

/*!
 * \brief Global object to interact with fluid medium (air, water, clouds...)
 *
 * This object handles the traversal of fluid medium and the collecting of
 * medium density and properties.
 * It is mainly used to compute the alteration made by such media on light.
 */
class SOFTWARESHARED_EXPORT FluidMediumTraversal
{
public:
    FluidMediumTraversal(SoftwareRenderer *renderer);

    void setCollector(FluidMediumCollector *collector);

    virtual void getTraversedMedia(std::vector<FluidMediumInterface> &media);

    void collectHalfLine(const SpaceCoordinates &start, const Vector3 &direction);
    void collectSegment(const SpaceCoordinates &start, const SpaceCoordinates &end);

private:
    SoftwareRenderer* renderer;
};

}
}

#endif // FLUIDMEDIUMTRAVERSAL_H
