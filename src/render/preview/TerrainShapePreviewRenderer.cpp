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

/*static void _getLightingStatus(Renderer*, LightStatus* status, Vector3, int)
{
    LightComponent light;

    light.color.r = 0.6;
    light.color.g = 0.6;
    light.color.b = 0.6;
    light.direction.x = -1.0;
    light.direction.y = -0.5;
    light.direction.z = 1.0;
    light.direction = v3Normalize(light.direction);
    light.altered = 1;
    light.reflection = 0.0;
    status->pushComponent(light);

    light.color.r = 0.2;
    light.color.g = 0.2;
    light.color.b = 0.2;
    light.direction.x = 1.0;
    light.direction.y = -0.5;
    light.direction.z = -1.0;
    light.direction = v3Normalize(light.direction);
    light.altered = 0;
    light.reflection = 0.0;
    status->pushComponent(light);
}

static Vector3 _getCameraLocation(Renderer*, Vector3 location)
{
    location.x -= 10.0;
    location.y += 15.0;
    location.z += 10.0;
    return location;
}*/

TerrainShapePreviewRenderer::TerrainShapePreviewRenderer(TerrainDefinition* terrain)
{
    _terrain = terrain;

    render_quality = 3;
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

    /*getCameraLocation = _getCameraLocation;
    atmosphere->getLightingStatus = _getLightingStatus;*/

    TextureLayerDefinition* layer = getScenery()->getTextures()->getTextureLayer(0);
    layer->terrain_zone->clear();
    layer->displacement_height = 0.0;
    layer->material->base = colorToHSL(COLOR_WHITE);
    layer->material->reflection = 0.05;
    layer->material->shininess = 2.0;
    layer->validate();
    layer->_detail_noise->clearLevels();
}

Color TerrainShapePreviewRenderer::getColor2D(double x, double y, double scaling)
{
    double height;

    height = getTerrainRenderer()->getHeight(x, y, 1);
    if (height > getTerrainRenderer()->getWaterHeight())
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
