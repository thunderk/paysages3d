#include "TerrainShapePreviewRenderer.h"

#include "TexturesDefinition.h"
#include "TextureLayerDefinition.h"
#include "Zone.h"
#include "SurfaceMaterial.h"
#include "NoiseGenerator.h"
#include "BasePreview.h"

// TEMP
#include "atmosphere/public.h"
#include "tools/lighting.h"
#include "textures/public.h"
#include "water/public.h"

static void _getLightingStatus(Renderer*, LightStatus* status, Vector3, int)
{
    LightDefinition light;

    light.color.r = 0.6;
    light.color.g = 0.6;
    light.color.b = 0.6;
    light.direction.x = -1.0;
    light.direction.y = -0.5;
    light.direction.z = 1.0;
    light.direction = v3Normalize(light.direction);
    light.altered = 1;
    light.reflection = 0.0;
    lightingPushLight(status, &light);

    light.color.r = 0.2;
    light.color.g = 0.2;
    light.color.b = 0.2;
    light.direction.x = 1.0;
    light.direction.y = -0.5;
    light.direction.z = -1.0;
    light.direction = v3Normalize(light.direction);
    light.altered = 0;
    light.reflection = 0.0;
    lightingPushLight(status, &light);
}

static Vector3 _getCameraLocation(Renderer*, Vector3 location)
{
    location.x -= 10.0;
    location.y += 15.0;
    location.z += 10.0;
    return location;
}

static void _alterPreviewRenderer(Renderer* renderer)
{
    renderer->render_quality = 3;
    renderer->getCameraLocation = _getCameraLocation;
    renderer->atmosphere->getLightingStatus = _getLightingStatus;

    TexturesDefinition textures(NULL);
    TextureLayerDefinition* layer = textures.getTextureLayer(textures.addLayer());
    layer->terrain_zone->clear();
    layer->displacement_height = 0.0;
    layer->material->base = colorToHSL(COLOR_WHITE);
    layer->material->reflection = 0.05;
    layer->material->shininess = 2.0;
    layer->validate();
    layer->_detail_noise->clearLevels();

    TexturesRendererClass.bind(renderer, &textures);
}

TerrainShapePreviewRenderer::TerrainShapePreviewRenderer(TerrainDefinition* terrain)
{
    _terrain = terrain;

    _alterPreviewRenderer(this);
}

void TerrainShapePreviewRenderer::bindEvent(BasePreview* preview)
{
    preview->addOsd(QString("geolocation"));

    preview->configScaling(20.0, 1000.0, 20.0, 200.0);
    preview->configScrolling(-1000.0, 1000.0, 0.0, -1000.0, 1000.0, 0.0);
}

void TerrainShapePreviewRenderer::updateEvent()
{
    TerrainRendererClass.bind(this, _terrain);
}

Color TerrainShapePreviewRenderer::getColor2D(double x, double y, double scaling)
{
    double height;

    height = terrain->getHeight(this, x, y, 1);
    if (height > terrain->getWaterHeight(this))
    {
        return terrain->getFinalColor(this, Vector3(x, height, y), 0.000001);
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
