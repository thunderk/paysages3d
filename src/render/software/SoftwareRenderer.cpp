#include "SoftwareRenderer.h"

#include "global/FluidMediumTraversal.h"

SoftwareRenderer::SoftwareRenderer(Scenery* scenery):
    scenery(scenery)
{
    fluid_medium = new FluidMediumTraversal(this);
}

SoftwareRenderer::~SoftwareRenderer()
{
    delete fluid_medium;
}
