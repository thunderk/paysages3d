#ifndef FLUIDMEDIUMMANAGER_H
#define FLUIDMEDIUMMANAGER_H

#include "software_global.h"

#include "SpaceSegment.h"

namespace paysages {
namespace software {

typedef struct {
    FluidMediumInterface *medium;
    SpaceSegment segment;
} FluidMediumSegment;

/*!
 * \brief Global object to interact with fluid medium (air, water, clouds...)
 *
 * This object handles the traversal of fluid medium and the collecting of
 * medium density and properties.
 * It is mainly used to compute the alteration made by such media on light.
 */
class SOFTWARESHARED_EXPORT FluidMediumManager {
  public:
    FluidMediumManager(SoftwareRenderer *renderer);
    virtual ~FluidMediumManager();

    /*!
     * \brief Remove all registered medium.
     */
    void clearMedia();

    /*!
     * \brief Register a new medium in the manager.
     */
    void registerMedium(FluidMediumInterface *medium);

    /*!
     * \brief Apply complete medium traversal
     * \param eye Position of the camera
     * \param location Point we look at
     * \param color Light initially received from 'location'
     * \return Light received by 'eye', transformed by medium traversal
     */
    virtual Color applyTraversal(const Vector3 &eye, const Vector3 &location, const Color &color) const;

    /*!
     * \brief Get the potential media traversed by a ray, unsorted
     */
    virtual int getTraversedMedia(FluidMediumSegment segments[], const SpaceSegment &ray, int max_segments) const;

  private:
    SoftwareRenderer *renderer;
    std::vector<FluidMediumInterface *> media;
};
}
}

#endif // FLUIDMEDIUMTRAVERSAL_H
