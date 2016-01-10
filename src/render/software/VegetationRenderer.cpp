#include "VegetationRenderer.h"

#include "VegetationModelRenderer.h"
#include "RayCastingResult.h"
#include "SpaceGridIterator.h"
#include "SpaceSegment.h"
#include "VegetationInstance.h"
#include "SoftwareRenderer.h"
#include "Scenery.h"
#include "TerrainRenderer.h"
#include "VegetationDefinition.h"
#include "VegetationLayerDefinition.h"
#include "VegetationPresenceDefinition.h"
#include "VegetationInstance.h"
#include "VegetationResult.h"
#include "LightComponent.h"

/**
 * Grid iterator to collect instances of a layer, in small squares.
 */
class VegetationGridIterator : public SpaceGridIterator {
  public:
    VegetationGridIterator(const SpaceSegment &segment, VegetationRenderer *renderer, bool only_hit)
        : segment(segment), renderer(renderer), only_hit(only_hit) {
    }

    inline const RayCastingResult &getResult() const {
        return result;
    }

    virtual bool onCell(int x, int, int z) override {
        result = renderer->getBoundResult(segment, to_double(x), to_double(z), only_hit);
        return not result.hit;
    }

  private:
    const SpaceSegment &segment;
    VegetationRenderer *renderer;
    RayCastingResult result;
    bool only_hit;
};

VegetationRenderer::VegetationRenderer(SoftwareRenderer *parent) : parent(parent) {
    enabled = true;
}

VegetationRenderer::~VegetationRenderer() {
}

void VegetationRenderer::setEnabled(bool enabled) {
    this->enabled = enabled;
}

RayCastingResult VegetationRenderer::renderInstance(const SpaceSegment &segment, const VegetationInstance &instance,
                                                    bool only_hit, bool displaced) {
    if (!displaced) {
        // Recursive call on displaced instance
        auto base = instance.getBase();
        auto terrain = parent->getTerrainRenderer()->getResult(base.x, base.z, true);
        auto displaced = parent->getTerrainRenderer()->getDisplaced(base.x, base.z, true);
        auto displaced_instance = instance.displace(displaced, terrain.normal);
        return renderInstance(segment, displaced_instance, only_hit, true);
    }

    RayCastingResult final;
    VegetationModelRenderer model_renderer(parent, &instance.getModel());
    SpaceSegment scaled_segment(segment.getStart().sub(instance.getBase()).scale(1.0 / instance.getSize()),
                                segment.getEnd().sub(instance.getBase()).scale(1.0 / instance.getSize()));
    VegetationResult result = model_renderer.getResult(scaled_segment, only_hit);

    final.hit = result.isHit();

    if (final.hit and not only_hit) {
        Vector3 location = result.getLocation().scale(instance.getSize()).add(instance.getBase());
        final.hit_color = parent->applyLightingToSurface(location, result.getNormal(), result.getMaterial());
        final.hit_color = parent->applyMediumTraversal(location, final.hit_color);
        final.hit_location = result.getLocation();
    }

    return final;
}

RayCastingResult VegetationRenderer::getResult(const SpaceSegment &segment, bool only_hit) {
    if (enabled) {
        // Find instances potentially crossing the segment
        VegetationGridIterator it(segment, this, only_hit);
        if (not segment.projectedOnYPlane().iterateOnGrid(it)) {
            return it.getResult();
        }
        return RayCastingResult();
    } else {
        return RayCastingResult();
    }
}

RayCastingResult VegetationRenderer::getBoundResult(const SpaceSegment &segment, double x, double z, bool only_hit,
                                                    double xsize, double zsize) {
    VegetationDefinition *vegetation = parent->getScenery()->getVegetation();

    // Early check if we may cross any vegetation
    double ymin, ymax;
    parent->getTerrainRenderer()->estimateMinMaxHeight(x, z, x + xsize, z + zsize, &ymin, &ymax);
    ymax += vegetation->getMaxHeight();
    SpaceSegment bbox(Vector3(x, ymin, z), Vector3(x + xsize, ymax, z + zsize));
    if (not segment.intersectBoundingBox(bbox)) {
        return RayCastingResult();
    }

    // Iterate all layers and instances
    int n = vegetation->getLayerCount();
    for (int i = 0; i < n; i++) {
        VegetationLayerDefinition *layer = vegetation->getVegetationLayer(i);

        vector<VegetationInstance> instances;
        layer->getPresence()->collectInstances(&instances, *layer->getModel(), x, z, x + xsize, z + zsize);

        for (auto &instance : instances) {
            RayCastingResult result = renderInstance(segment, instance, only_hit);
            if (result.hit) {
                // TODO Don't stop at first hit, find the nearest one
                return result;
            }
        }
    }

    return RayCastingResult();
}

bool VegetationRenderer::applyLightFilter(LightComponent &light, const Vector3 &at) {
    if (enabled) {
        // Get segment to iterate
        SpaceSegment segment(at, at.add(light.direction.scale(-1.0 * parent->render_quality)));
        if (getResult(segment, true).hit) {
            light.color = COLOR_BLACK;
            return false;
        } else {
            return true;
        }
    } else {
        return true;
    }
}
