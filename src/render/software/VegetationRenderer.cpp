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
#include "VegetationResult.h"
#include "LightComponent.h"

const double DEBUG_DENSITY_FACTOR = 0.5;

// TEMP
class VegetationGridIterator: public SpaceGridIterator
{
public:
    VegetationGridIterator(const SpaceSegment &segment, VegetationRenderer *renderer, VegetationModelDefinition *model, bool only_hit):
        segment(segment), renderer(renderer), model(model), only_hit(only_hit)
    {
    }

    inline const RayCastingResult &getResult() const {return result;}

    virtual bool onCell(int x, int, int z) override
    {
        double dx = ((double)x + 0.5) * DEBUG_DENSITY_FACTOR;
        double dz = ((double)z + 0.5) * DEBUG_DENSITY_FACTOR;
        Vector3 base = renderer->getParent()->getTerrainRenderer()->getResult(dx, dz, 1, 1).location;
        VegetationInstance instance(*model, base, 0.2);
        result = renderer->renderInstance(segment, instance, only_hit);
        return not result.hit;
    }
private:
    const SpaceSegment &segment;
    VegetationRenderer *renderer;
    VegetationModelDefinition *model;
    RayCastingResult result;
    bool only_hit;
};

VegetationRenderer::VegetationRenderer(SoftwareRenderer *parent):
    parent(parent)
{
}

void VegetationRenderer::setEnabled(bool enabled)
{
    this->enabled = enabled;
}

RayCastingResult VegetationRenderer::renderInstance(const SpaceSegment &segment, const VegetationInstance &instance, bool only_hit)
{
    RayCastingResult final;

    VegetationModelRenderer model_renderer(parent, &instance.getModel());
    SpaceSegment scaled_segment(segment.getStart().sub(instance.getBase()).scale(1.0 / instance.getSize()),
                                segment.getEnd().sub(instance.getBase()).scale(1.0 / instance.getSize()));
    VegetationResult result = model_renderer.getResult(scaled_segment, only_hit);

    final.hit = result.isHit();

    if (final.hit and not only_hit)
    {
        Vector3 location = result.getLocation().scale(instance.getSize()).add(instance.getBase());
        final.hit_color = parent->applyLightingToSurface(location, result.getNormal(), result.getMaterial());
        final.hit_color = parent->applyMediumTraversal(location, final.hit_color);
        final.hit_location = result.getLocation();
    }

    return final;
}

RayCastingResult VegetationRenderer::getResult(const SpaceSegment &segment, bool only_hit)
{
    if (enabled)
    {
        // Find instances potentially crossing the segment
        // TODO Collect the nearest hit, don't stop at the first one
        VegetationGridIterator it(segment, this, parent->getScenery()->getVegetation()->debug_model, only_hit);
        if (not segment.projectedOnYPlane().scaled(1.0 / DEBUG_DENSITY_FACTOR).iterateOnGrid(it))
        {
            return it.getResult();
        }
        else
        {
            return RayCastingResult();
        }
    }
    else
    {
        return RayCastingResult();
    }
}

bool VegetationRenderer::applyLightFilter(LightComponent &light, const Vector3 &at)
{
    if (enabled)
    {
        // Get segment to iterate
        SpaceSegment segment(at, at.add(light.direction.scale(-1.0 * parent->render_quality)));
        if (getResult(segment, true).hit)
        {
            light.color = COLOR_BLACK;
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        return true;
    }
}
