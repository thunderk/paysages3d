#ifndef FLUIDMEDIUMMANAGER_H
#define FLUIDMEDIUMMANAGER_H

#include "software_global.h"

#include "SpaceSegment.h"

#include <vector>

namespace paysages {
namespace software {

typedef struct {
    FluidMediumInterface *medium;
    SpaceSegment segment;
} FluidMediumSegment;

/**
 * Global object to interact with fluid medium (air, water, clouds...)
 *
 * This object handles the traversal of fluid medium and the collecting of
 * medium density and properties.
 * It is mainly used to compute the alteration made by such media on light.
 */
class SOFTWARESHARED_EXPORT FluidMediumManager {
  public:
    FluidMediumManager(SoftwareRenderer *renderer);
    virtual ~FluidMediumManager();

    /**
     * Remove all registered medium.
     */
    void clearMedia();

    /**
     * Register a new medium in the manager.
     */
    void registerMedium(FluidMediumInterface *medium);

    /**
     * Apply complete medium traversal between *location* and *eye*, to the base *color*.
     *
     * Returns the light received by eye, transformed by medium traversal.
     */
    virtual Color applyTraversal(const Vector3 &eye, const Vector3 &location, const Color &color) const;

    /**
     * Get the potential media traversed by a ray, unsorted
     */
    virtual int getTraversedMedia(FluidMediumSegment segments[], const SpaceSegment &ray, int max_segments) const;

  private:
    SoftwareRenderer *renderer;
    vector<FluidMediumInterface *> media;
};
}
}

#endif // FLUIDMEDIUMTRAVERSAL_H
