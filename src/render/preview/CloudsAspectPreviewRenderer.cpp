#include "CloudsAspectPreviewRenderer.h"

#include "clouds/public.h"
#include "atmosphere/public.h"
#include "BasePreview.h"
#include "Scenery.h"
#include "CloudsDefinition.h"
#include "CloudLayerDefinition.h"

static void _getLightingStatus(Renderer*, LightStatus* status, Vector3, int)
{
    LightDefinition light;

    light.color.r = 0.5;
    light.color.g = 0.5;
    light.color.b = 0.5;
    light.direction = Vector3(-1.0, 0.5, 1.0).normalize();
    light.altered = 1;
    light.reflection = 0.0;
    lightingPushLight(status, &light);

    light.color.r = 0.1;
    light.color.g = 0.1;
    light.color.b = 0.1;
    light.direction = Vector3(1.0, -0.5, -1.0).normalize();
    light.altered = 0;
    light.reflection = 0.0;
    lightingPushLight(status, &light);
}

static double _getDensity(Renderer*, CloudLayerDefinition* layer, Vector3 location)
{
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

static AtmosphereResult _fakeApplyAerialPerspective(Renderer*, Vector3, Color base)
{
    AtmosphereResult result;

    result.base = base;
    result.final = base;

    return result;
}

CloudsAspectPreviewRenderer::CloudsAspectPreviewRenderer(CloudLayerDefinition* layer):
    layer(layer)
{
    CloudsDefinition* clouds = getScenery()->getClouds();
    clouds->clear();
    clouds->addLayer();
    CloudsRendererClass.bind(this, clouds);

    render_quality = 6;
}

void CloudsAspectPreviewRenderer::bindEvent(BasePreview* preview)
{
    preview->configScaling(0.5, 2.0, 0.1, 2.0);
}

void CloudsAspectPreviewRenderer::updateEvent()
{
    CloudLayerDefinition* preview_layer = getScenery()->getClouds()->getCloudLayer(0);
    layer->copy(preview_layer);

    preview_layer->thickness = preview_layer->shape_scaling;
    preview_layer->lower_altitude = -preview_layer->thickness / 2.0;
    preview_layer->validate();

    prepare();

    clouds->getLayerDensity = _getDensity;
    atmosphere->getLightingStatus = _getLightingStatus;
    atmosphere->applyAerialPerspective = _fakeApplyAerialPerspective;
}

Color CloudsAspectPreviewRenderer::getColor2D(double x, double y, double)
{
    Vector3 start, end;
    double thickness = layer->thickness;

    start.x = x * thickness * 0.5;
    start.z = y * thickness * 0.5;
    start.y = thickness * 0.5;

    end.x = start.x;
    end.z = start.z;
    end.y = -start.y;

    return clouds->getColor(this, COLOR_BLUE, start, end);
}
