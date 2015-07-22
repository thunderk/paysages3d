#include "TerrainShapePreviewRenderer.h"

#include "TexturesDefinition.h"
#include "TextureLayerDefinition.h"
#include "Zone.h"
#include "SurfaceMaterial.h"
#include "NoiseGenerator.h"
#include "BasePreview.h"
#include "Scenery.h"
#include "LightComponent.h"
#include "LightStatus.h"
#include "TerrainRenderer.h"

TerrainShapePreviewRenderer::TerrainShapePreviewRenderer(TerrainDefinition* terrain)
{
    _terrain = terrain;

    render_quality = 3;

    disableClouds();

    getScenery()->getTextures()->clear();
    getScenery()->getTextures()->addLayer();
    TextureLayerDefinition* layer = getScenery()->getTextures()->getTextureLayer(0);
    layer->terrain_zone->clear();
    layer->displacement_height = 0.0;
    layer->material->base = colorToHSL(COLOR_WHITE);
    layer->material->reflection = 0.05;
    layer->material->shininess = 2.0;
    layer->validate();
    layer->_detail_noise->clearLevels();
}

Vector3 TerrainShapePreviewRenderer::getCameraLocation(const Vector3 &target)
{
    return target.add(Vector3(-10.0, 15.0, 10.0));
}

void TerrainShapePreviewRenderer::bindEvent(BasePreview* preview)
{
    preview->addOsd(QString("geolocation"));

    preview->configScaling(20.0, 1000.0, 20.0, 200.0);
    preview->configScrolling(-1000.0, 1000.0, 0.0, -1000.0, 1000.0, 0.0);
}

void TerrainShapePreviewRenderer::updateEvent()
{
    getScenery()->setTerrain(_terrain);

    prepare();

    disableAtmosphere();
}

Color TerrainShapePreviewRenderer::getColor2D(double x, double y, double scaling)
{
    double height;

    height = getTerrainRenderer()->getHeight(x, y, true);
    if (height > 0.0)
    {
        return getTerrainRenderer()->getFinalColor(Vector3(x, height, y), 0.000001);
    }
    else
    {
        return getWaterColor(x, y, scaling);
    }
}

Color TerrainShapePreviewRenderer::getWaterColor(double, double, double)
{
    return COLOR_BLUE;
}
