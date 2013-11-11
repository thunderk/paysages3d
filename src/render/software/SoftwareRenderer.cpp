#include "SoftwareRenderer.h"

#include "camera.h"
#include "Scenery.h"
#include "FluidMediumManager.h"

SoftwareRenderer::SoftwareRenderer(Scenery* scenery)
{
    fluid_medium = new FluidMediumManager(this);

    if (scenery)
    {
        this->scenery = scenery;
    }
    else
    {
        this->scenery = Scenery::getCurrent();
    }
    this->scenery->bindToRenderer(this);
}

SoftwareRenderer::~SoftwareRenderer()
{
    delete fluid_medium;
}

void SoftwareRenderer::initialize()
{
    //fluid_medium->registerMedium(water_renderer);
}

/*Color SoftwareRenderer::applyMediumTraversal(Vector3 location, Color color)
{
    Vector3 eye = cameraGetLocation(scenery->getCamera());
    return fluid_medium->applyTraversal(eye, location, color);
}*/
