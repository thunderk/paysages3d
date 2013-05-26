#include "private.h"

/*
 * Clouds tools.
 */

#include "clo_walking.h"
#include "../renderer.h"
#include "../tools.h"

static inline Vector3 _getNormal(CloudsLayerDefinition* layer, Vector3 position, double detail)
{
    Vector3 result = {0.0, 0.0, 0.0};
    /*Vector3 dposition;
    double val, dval;

    val = _getDistanceToBorder(layer, position);

    dposition.x = position.x + detail;
    dposition.y = position.y;
    dposition.z = position.z;
    dval = val - _getDistanceToBorder(layer, dposition);
    result.x += dval;

    dposition.x = position.x - detail;
    dval = val - _getDistanceToBorder(layer, dposition);
    result.x -= dval;

    dposition.x = position.x;
    dposition.y = position.y + detail;
    dval = val - _getDistanceToBorder(layer, dposition);
    result.y += dval;

    dposition.y = position.y - detail;
    dval = val - _getDistanceToBorder(layer, dposition);
    result.y -= dval;

    dposition.y = position.y;
    dposition.z = position.z + detail;
    dval = val - _getDistanceToBorder(layer, dposition);
    result.z += dval;

    dposition.z = position.z - detail;
    dval = val - _getDistanceToBorder(layer, dposition);
    result.z -= dval;*/

    return v3Normalize(result);
}

static Color _applyLayerLighting(CloudsLayerDefinition* definition, Renderer* renderer, Vector3 location, double detail)
{
    Vector3 normal;
    Color col1, col2;
    LightStatus* lighting;

    normal = _getNormal(definition, location, 3.0);
    if (renderer->render_quality > 3)
    {
        normal = v3Add(normal, _getNormal(definition, location, 2.0));
        normal = v3Add(normal, _getNormal(definition, location, 1.0));
    }
    if (renderer->render_quality > 5)
    {
        normal = v3Add(normal, _getNormal(definition, location, 0.5));
    }
    if (renderer->render_quality > 8)
    {
        normal = v3Add(normal, _getNormal(definition, location, 0.75));
        normal = v3Add(normal, _getNormal(definition, location, 1.25));
        normal = v3Add(normal, _getNormal(definition, location, 2.5));
    }
    normal = v3Scale(v3Normalize(normal), definition->hardness);

    return renderer->applyLightingToSurface(renderer, location, normal, &definition->material);

    lighting = lightingCreateStatus(renderer->lighting, location, renderer->getCameraLocation(renderer, location));
    renderer->atmosphere->getLightingStatus(renderer, lighting, normal, 0);
    col1 = lightingApplyStatus(lighting, normal, &definition->material);
    col2 = lightingApplyStatus(lighting, v3Scale(normal, -1.0), &definition->material);
    lightingDeleteStatus(lighting);

    col1.r = (col1.r + col2.r) / 2.0;
    col1.g = (col1.g + col2.g) / 2.0;
    col1.b = (col1.b + col2.b) / 2.0;
    col1.a = (col1.a + col2.a) / 2.0;

    return col1;
}

Color cloudsApplyLayer(CloudsLayerDefinition* definition, Color base, Renderer* renderer, Vector3 start, Vector3 end)
{
    Color col = COLOR_TRANSPARENT;

    if (!cloudsOptimizeWalkingBounds(definition, &start, &end))
    {
        return base;
    }

    /* TODO Walk through the cloud */

    /*segment_count = cloudsGetLayerPrimarySegments(renderer, definition, start, end, MAX_SEGMENT_COUNT, segments);*/

    col = renderer->atmosphere->applyAerialPerspective(renderer, start, col).final;
    col.a = 0.0;

    colorMask(&base, &col);

    return base;
}

Color cloudsLayerFilterLight(CloudsLayerDefinition* definition, Renderer* renderer, Color light, Vector3 location, Vector3 light_location, Vector3 direction_to_light)
{
    /*double inside_depth, total_depth, factor;
    CloudSegment segments[MAX_SEGMENT_COUNT];

    if (!cloudsOptimizeWalkingBounds(definition, &location, &light_location))
    {
        return light;
    }

    _getPrimarySegments(definition, renderer, location, direction_to_light, MAX_SEGMENT_COUNT, definition->lighttraversal, v3Norm(v3Sub(light_location, location)), &inside_depth, &total_depth, segments);

    if (definition->lighttraversal < 0.0001)
    {
        factor = 0.0;
    }
    else
    {
        factor = inside_depth / definition->lighttraversal;
        if (factor > 1.0)
        {
            factor = 1.0;
        }
    }

    factor = 1.0 - (1.0 - definition->minimumlight) * factor;

    light.r = light.r * factor;
    light.g = light.g * factor;
    light.b = light.b * factor;*/

    return light;
}
