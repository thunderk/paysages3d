#include "CloudsCoveragePreviewRenderer.h"

#include "clouds/public.h"
#include "BasePreview.h"
#include "Scenery.h"
#include "CloudsDefinition.h"
#include "CloudLayerDefinition.h"

Color _fakeApplyLightingToSurface(Renderer*, Vector3, Vector3, SurfaceMaterial*)
{
    return COLOR_WHITE;
}

CloudsCoveragePreviewRenderer::CloudsCoveragePreviewRenderer(CloudLayerDefinition* layer):
    layer(layer)
{
    perspective = true;

    CloudsDefinition* clouds = getScenery()->getClouds();
    clouds->clear();
    clouds->addLayer();
    CloudsRendererClass.bind(this, clouds);
}

void CloudsCoveragePreviewRenderer::bindEvent(BasePreview* preview)
{
    preview->addToggle("3d", "Perspective", perspective);
    preview->configScaling(100.0, 1000.0, 20.0, 200.0);
}

void CloudsCoveragePreviewRenderer::updateEvent()
{
    layer->copy(getScenery()->getClouds()->getCloudLayer(0));
    prepare();
    applyLightingToSurface = _fakeApplyLightingToSurface;
}

Color CloudsCoveragePreviewRenderer::getColor2D(double x, double y, double scaling)
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
        look = look.normalize();

        return clouds->getColor(this, COLOR_BLUE, eye, eye.add(look.scale(1000.0)));
    }
    else
    {
        Vector3 start, end;

        start.x = end.x = x;
        start.z = end.z = y;
        start.y = 1000.0;
        end.y = -1000.0;

        return clouds->getColor(this, COLOR_BLUE, start, end);
    }
}

void CloudsCoveragePreviewRenderer::toggleChangeEvent(const std::string &key, bool value)
{
    if (key == "3d")
    {
        perspective = value;
    }
}
