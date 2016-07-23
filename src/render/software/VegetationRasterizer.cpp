#include "VegetationRasterizer.h"

#include "CanvasFragment.h"
#include "Color.h"
#include "RayCastingResult.h"
#include "Scenery.h"
#include "SoftwareRenderer.h"
#include "SpaceSegment.h"
#include "VegetationDefinition.h"
#include "VegetationRenderer.h"
#include <cassert>

VegetationRasterizer::VegetationRasterizer(SoftwareRenderer *renderer, RenderProgress *progress,
                                           unsigned short client_id)
    : TerrainRasterizer(renderer, progress, client_id) {
    setYOffset(0.5);
    setColor(Color(0.7, 1.0, 0.7, 0.5));
}

bool VegetationRasterizer::isUseful() const {
    return renderer->getScenery()->getVegetation()->getMaxHeight() > 0.0;
}

int VegetationRasterizer::prepareRasterization() {
    if (isUseful()) {
        return TerrainRasterizer::prepareRasterization();
    } else {
        return 0;
    }
}

void VegetationRasterizer::rasterizeToCanvas(CanvasPortion *canvas) {
    if (isUseful()) {
        TerrainRasterizer::rasterizeToCanvas(canvas);
    }
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
