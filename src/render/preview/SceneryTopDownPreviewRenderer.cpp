#include "SceneryTopDownPreviewRenderer.h"

#include "CloudsDefinition.h"
#include "BasePreview.h"
#include "Scenery.h"
#include "TerrainRenderer.h"
#include "WaterRenderer.h"
#include "CloudsRenderer.h"

SceneryTopDownPreviewRenderer::SceneryTopDownPreviewRenderer(Scenery* scenery):
    scenery(scenery)
{
    clouds_enabled = true;
}

void SceneryTopDownPreviewRenderer::bindEvent(BasePreview* preview)
{
    no_clouds = new CloudsDefinition(NULL);
    clouds_enabled = true;

    // TODO Translation
    preview->addOsd("geolocation");
    preview->addToggle("clouds", "Clouds", false);

    preview->configHdrToneMapping(true);
    preview->configScaling(0.5, 200.0, 3.0, 50.0);
    preview->configScrolling(-1000.0, 1000.0, 0.0, -1000.0, 1000.0, 0.0);
}

void SceneryTopDownPreviewRenderer::updateEvent()
{
    setScenery(scenery);

    if (not clouds_enabled)
    {
        getScenery()->getClouds()->clear();
    }

    prepare();
}

Color SceneryTopDownPreviewRenderer::getColor2D(double x, double y, double scaling)
{
    Vector3 location;
    double height = getTerrainRenderer()->getHeight(x, y, 1);

    if (height < getWaterRenderer()->getHeightInfo().max_height)
    {
        return getWaterRenderer()->getResult(x, y).final;
    }
    else
    {
        location.x = x;
        location.y = height;
        location.z = y;
        return getTerrainRenderer()->getFinalColor(location, scaling);
    }
}

void SceneryTopDownPreviewRenderer::toggleChangeEvent(const std::string &key, bool value)
{
    if (key == "clouds")
    {
        clouds_enabled = value;
    }
}

Vector3 SceneryTopDownPreviewRenderer::getCameraLocation(const Vector3 &target)
{
    return target.add(VECTOR_UP.scale(50.0));
}

Color SceneryTopDownPreviewRenderer::applyMediumTraversal(Vector3 location, Color color)
{
    if (clouds_enabled)
    {
        return getCloudsRenderer()->getColor(getCameraLocation(location), location, color);
    }
    else
    {
        return color;
    }
}

