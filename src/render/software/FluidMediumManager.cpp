#include "FluidMediumManager.h"

#include "SoftwareRenderer.h"
#include "FluidMediumInterface.h"

FluidMediumManager::FluidMediumManager(SoftwareRenderer* renderer):
    renderer(renderer)
{
}

FluidMediumManager::~FluidMediumManager()
{
}

void FluidMediumManager::clearMedia()
{
    media.clear();
}

void FluidMediumManager::registerMedium(FluidMediumInterface *medium)
{
    media.push_back(medium);
}

Color FluidMediumManager::applyTraversal(const Vector3 &eye, const Vector3 &location, const Color &color) const
{
    // Collect potential segments
    SpaceSegment ray(eye, location);
    int max_segments = media.size();
    FluidMediumSegment* segments;
    segments = new FluidMediumSegment[max_segments];
    getTraversedMedia(segments, ray, max_segments);

    delete[] segments;
    return color;
}

int FluidMediumManager::getTraversedMedia(FluidMediumSegment segments[], const SpaceSegment &ray, int max_segments) const
{
    int added = 0;
    for (auto &medium : media)
    {
        SpaceSegment ray_inter(ray);
        if (added < max_segments and medium->checkInfluence(ray_inter))
        {
            // The medium intersect with the ray
            segments[added++] = {medium, ray_inter};
        }
    }
    return added;
}
