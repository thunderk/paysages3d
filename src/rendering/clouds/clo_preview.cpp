#include "clo_preview.h"
#include "private.h"

#include "../tools/euclid.h"
#include "../renderer.h"
#include "../tools.h"
#include "rendering/atmosphere/public.h"

/*
 * Clouds previews.
 */

Color _fakeApplyLightingToSurface(Renderer* renderer, Vector3 location, Vector3 normal, SurfaceMaterial* material)
{
    UNUSED(renderer);
    UNUSED(location);
    UNUSED(normal);
    UNUSED(material);

    return COLOR_WHITE;
}

Renderer* cloudsPreviewCoverageCreateRenderer()
{
    Renderer* result = rendererCreate();
    result->render_quality = 5;
    result->applyLightingToSurface = _fakeApplyLightingToSurface;
    return result;
}

void cloudsPreviewCoverageBindLayer(Renderer* renderer, CloudsLayerDefinition* layer)
{
    CloudsDefinition* definition = (CloudsDefinition*)CloudsDefinitionClass.create();
    layersAddLayer(definition->layers, layer);
    CloudsRendererClass.bind(renderer, definition);
    CloudsDefinitionClass.destroy(definition);
}

Color cloudsPreviewCoverageGetPixel(Renderer* renderer, double x, double y, double scaling, int perspective)
{
    if (perspective)
    {
        Vector3 eye, look;

        eye.x = 0.0;
        eye.y = scaling;
        eye.z = -10.0 * scaling;
        look.x = x * 0.01 / scaling;
        look.y = -(y * 0.01 - 0.3) / scaling;
        look.z = 1.0;
        look = v3Normalize(look);

        return renderer->clouds->getColor(renderer, COLOR_BLUE, eye, v3Add(eye, v3Scale(look, 1000.0)));
    }
    else
    {
        Vector3 start, end;

        start.x = end.x = x;
        start.z = end.z = y;
        start.y = 1000.0;
        end.y = -1000.0;

        return renderer->clouds->getColor(renderer, COLOR_BLUE, start, end);
    }
}

static void _getLightingStatus(Renderer* renderer, LightStatus* status, Vector3 normal, int opaque)
{
    LightDefinition light;

    UNUSED(renderer);
    UNUSED(normal);
    UNUSED(opaque);

    light.color.r = 0.5;
    light.color.g = 0.5;
    light.color.b = 0.5;
    light.direction.x = -1.0;
    light.direction.y = 0.5;
    light.direction.z = 1.0;
    light.direction = v3Normalize(light.direction);
    light.altered = 1;
    light.reflection = 0.0;
    lightingPushLight(status, &light);

    light.color.r = 0.1;
    light.color.g = 0.1;
    light.color.b = 0.1;
    light.direction.x = 1.0;
    light.direction.y = -0.5;
    light.direction.z = -1.0;
    light.direction = v3Normalize(light.direction);
    light.altered = 0;
    light.reflection = 0.0;
    lightingPushLight(status, &light);
}

Renderer* cloudsPreviewMaterialCreateRenderer()
{
    Renderer* result = rendererCreate();
    result->render_quality = 6;
    result->atmosphere->getLightingStatus = _getLightingStatus;
    return result;
}

static double _getDensity(Renderer* renderer, CloudsLayerDefinition* layer, Vector3 location)
{
    UNUSED(renderer);
    UNUSED(layer);

    double distance = 2.0 * v3Norm(location) / layer->thickness;
    if (distance > 1.0)
    {
        return 0.0;
    }
    else if (distance < 0.8)
    {
        return 1.0;
    }
    else
    {
        return (1.0 - distance) / 0.2;
    }
}

void cloudsPreviewMaterialBindLayer(Renderer* renderer, CloudsLayerDefinition* layer)
{
    CloudsDefinition* definition = (CloudsDefinition*)CloudsDefinitionClass.create();
    layersAddLayer(definition->layers, layer);
    CloudsRendererClass.bind(renderer, definition);
    CloudsDefinitionClass.destroy(definition);

    layer = (CloudsLayerDefinition*)layersGetLayer(renderer->clouds->definition->layers, 0);
    layer->thickness = layer->shape_scaling;
    layer->lower_altitude = -layer->thickness / 2.0;

    renderer->clouds->getLayerDensity = _getDensity;
}

Color cloudsPreviewMaterialGetPixel(Renderer* renderer, double x, double y)
{
    Vector3 start, end;
    CloudsLayerDefinition* layer = (CloudsLayerDefinition*)layersGetLayer(renderer->clouds->definition->layers, 0);
    double thickness = layer->thickness;

    start.x = x * thickness * 0.5;
    start.z = y * thickness * 0.5;
    start.y = thickness * 0.5;

    end.x = start.x;
    end.z = start.z;
    end.y = -start.y;

    return renderer->clouds->getColor(renderer, COLOR_BLUE, start, end);
}
