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

    virtual void initialize();

    inline Scenery* getScenery() const {return scenery;}
    inline FluidMediumManager* getFluidMediumManager() const {return fluid_medium;}

    //virtual Color applyMediumTraversal(Vector3 location, Color color) override;

private:
    Scenery* scenery;
    FluidMediumManager* fluid_medium;
};

}
}

#endif // SOFTWARERENDERER_H
