#ifndef SOFTWARERENDERER_H
#define SOFTWARERENDERER_H

#include "software_global.h"

#include "renderer.h"

class Scenery;

namespace paysages {
namespace software {

class FluidMediumTraversal;

/*!
 * \brief This class renders a defined scenery in sotware mode (using only standard CPU computations).
 */
class SOFTWARESHARED_EXPORT SoftwareRenderer: public Renderer
{

public:
    SoftwareRenderer(Scenery* scenery);
    virtual ~SoftwareRenderer();

    inline Scenery* getScenery() const {return scenery;}

private:
    Scenery* scenery;
    FluidMediumTraversal* fluid_medium;
};

}
}

#endif // SOFTWARERENDERER_H
