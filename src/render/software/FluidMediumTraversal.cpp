#include "FluidMediumTraversal.h"

FluidMediumTraversal::FluidMediumTraversal(SoftwareRenderer* renderer):
    renderer(renderer)
{
}

FluidMediumTraversal::~FluidMediumTraversal()
{
}

void FluidMediumTraversal::setCollector(FluidMediumCollector *collector)
{
}

void FluidMediumTraversal::getTraversedMedia(std::vector<FluidMediumInterface> &media, const SpaceCoordinates &start, const SpaceCoordinates &end)
{
}

void FluidMediumTraversal::collectHalfLine(const SpaceCoordinates &start, const Vector3 &direction)
{
}

void FluidMediumTraversal::collectSegment(const SpaceCoordinates &start, const SpaceCoordinates &end)
{
}
