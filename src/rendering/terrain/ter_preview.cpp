#include "public.h"

#include "../tools.h"
#include "../tools/lighting.h"
#include "../renderer.h"
#include "NoiseGenerator.h"
#include "atmosphere/public.h"
#include "textures/public.h"
#include "TextureLayerDefinition.h"
#include "TexturesDefinition.h"
#include "Zone.h"

/*
 * Terrain previews.
 */

static void _getLightingStatus(Renderer* renderer, LightStatus* status, Vector3 normal, int opaque)
{
    LightDefinition light;

    UNUSED(renderer);
    UNUSED(normal);
    UNUSED(opaque);

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

static Vector3 _getCameraLocation(Renderer* renderer, Vector3 location)
{
    UNUSED(renderer);
    location.x -= 10.0;
    location.y += 15.0;
    location.z += 10.0;
    return location;
}

void terrainAlterPreviewRenderer(Renderer* renderer)
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

Color terrainGetPreviewColor(Renderer* renderer, double x, double z, double detail)
{
    Vector3 point;

    point.x = x;
    point.y = renderer->terrain->getHeight(renderer, x, z, 1);
    point.z = z;

    return renderer->terrain->getFinalColor(renderer, point, detail);
}
