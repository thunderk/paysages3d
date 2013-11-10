#include "SoftwareRenderer.h"

#include "Scenery.h"
#include "FluidMediumTraversal.h"

SoftwareRenderer::SoftwareRenderer(Scenery* scenery)
{
    fluid_medium = new FluidMediumTraversal(this);

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

Color SoftwareRenderer::applyMediumTraversal(Vector3 location, Color color)
{
    // TODO
    return color;
}
