#ifndef SOFTWARERENDERER_H
#define SOFTWARERENDERER_H

#include "software_global.h"

#include "renderer.h"

class Scenery;

namespace paysages {
namespace software {

/*!
 * \brief This class renders a defined scenery in sotware mode (using only standard CPU computations).
 */
class SOFTWARESHARED_EXPORT SoftwareRenderer: public Renderer
{

public:
    SoftwareRenderer(Scenery* scenery=0);
    virtual ~SoftwareRenderer();

    /*!
     * \brief Prepare the renderer sub-systems.
     *
     * This will clear the caches and connect elements together.
     * After this call, don't update the scenery when renderer is in use.
     */
    virtual void prepare();

    inline Scenery* getScenery() const {return scenery;}
    inline BaseAtmosphereRenderer* getAtmosphereRenderer() const {return atmosphere_renderer;}
    inline FluidMediumManager* getFluidMediumManager() const {return fluid_medium;}

    //virtual Color applyMediumTraversal(Vector3 location, Color color) override;

private:
    Scenery* scenery;
    FluidMediumManager* fluid_medium;
    BaseAtmosphereRenderer* atmosphere_renderer;
};

}
}

#endif // SOFTWARERENDERER_H
