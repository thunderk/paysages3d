#include "VegetationRasterizer.h"

#include <cassert>

#include "CanvasFragment.h"
#include "Color.h"
#include "SpaceSegment.h"
#include "SoftwareRenderer.h"
#include "VegetationRenderer.h"
#include "RayCastingResult.h"

VegetationRasterizer::VegetationRasterizer(SoftwareRenderer *renderer, RenderProgress *progress, int client_id)
    : TerrainRasterizer(renderer, progress, client_id) {
    setYOffset(0.5);
    setColor(Color(0.8, 1.0, 0.8, 0.5));
}

Color VegetationRasterizer::shadeFragment(const CanvasFragment &fragment, const CanvasFragment *previous) const {
    assert(previous != NULL);

    if (not fragment.isFrontFacing() or previous->getClient() == RASTERIZER_CLIENT_SKY) {
        // This is an exit fragment, or the last before sky
        return COLOR_TRANSPARENT;
    }

    // Even if we assert, this may happen in rare circumstances (no opaque background fragment), so don't crash
    if (previous == NULL) {
        return COLOR_TRANSPARENT;
    }

    SoftwareRenderer *renderer = getRenderer();
    SpaceSegment segment(renderer->unprojectPoint(fragment.getPixel()), renderer->unprojectPoint(previous->getPixel()));
    RayCastingResult result = renderer->getVegetationRenderer()->getResult(segment);
    if (result.hit) {
        return result.hit_color;
    } else {
        return COLOR_TRANSPARENT;
    }
}
