#include "CloudsCoveragePreviewRenderer.h"

#include "BasePreview.h"
#include "Scenery.h"
#include "CloudsDefinition.h"
#include "CloudLayerDefinition.h"
#include "CloudsRenderer.h"

CloudsCoveragePreviewRenderer::CloudsCoveragePreviewRenderer(CloudLayerDefinition* layer):
    layer(layer)
{
    perspective = true;

    CloudsDefinition* clouds = getScenery()->getClouds();
    clouds->clear();
    clouds->addLayer();
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

        return getCloudsRenderer()->getColor(eye, eye.add(look.scale(1000.0)), COLOR_BLUE);
    }
    else
    {
        Vector3 start, end;

        start.x = end.x = x;
        start.z = end.z = y;
        start.y = 1000.0;
        end.y = -1000.0;

        return getCloudsRenderer()->getColor(start, end, COLOR_BLUE);
    }
}

void CloudsCoveragePreviewRenderer::toggleChangeEvent(const std::string &key, bool value)
{
    if (key == "3d")
    {
        perspective = value;
    }
}

Color CloudsCoveragePreviewRenderer::applyLightingToSurface(const Vector3 &, const Vector3 &, const SurfaceMaterial &)
{
    return COLOR_WHITE;
}
