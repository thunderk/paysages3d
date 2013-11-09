#include "SoftwareRenderer.h"

#include "FluidMediumTraversal.h"

SoftwareRenderer::SoftwareRenderer(Scenery* scenery):
    scenery(scenery)
{
    fluid_medium = new FluidMediumTraversal(this);
}

SoftwareRenderer::~SoftwareRenderer()
{
    delete fluid_medium;
}
