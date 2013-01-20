#include "public.h"

#include <stdlib.h>
#include "../tools/lighting.h"
#include "../renderer.h"
#include "../textures.h"

/*
 * Terrain previews.
 */

static TexturesDefinition _textures;
/*static LightingDefinition _lighting;*/
static int _inited = 0;

static Color _applyTextures(Renderer* renderer, Vector3 location, double precision)
{
    return texturesGetColor(&_textures, renderer, location.x, location.z, precision);
}

Renderer* terrainCreatePreviewRenderer()
{
    Renderer* result = rendererCreate();

    result->render_quality = 3;
    result->applyTextures = _applyTextures;
    result->camera_location.x = 0.0;
    result->camera_location.y = 50.0;
    result->camera_location.z = 0.0;

    if (!_inited)
    {
        /*LightDefinition light;*/
        TextureLayerDefinition* texture;

        _inited = 1;

        /*_lighting = lightingCreateDefinition();
        light.color.r = 0.6;
        light.color.g = 0.6;
        light.color.b = 0.6;
        light.direction.x = -1.0;
        light.direction.y = -0.5;
        light.direction.z = -1.0;
        light.direction = v3Normalize(light.direction);
        light.filtered = 0;
        light.masked = 1;
        light.reflection = 1.0;
        lightingAddLight(&_lighting, light);
        light.color.r = 0.3;
        light.color.g = 0.3;
        light.color.b = 0.3;
        light.direction.x = 0.5;
        light.direction.y = 0.7071;
        light.direction.z = 0.5;
        light.filtered = 0;
        light.masked = 0;
        light.reflection = 0.0;
        lightingAddLight(&_lighting, light);
        lightingValidateDefinition(&_lighting);*/

        _textures = texturesCreateDefinition();
        texture = (TextureLayerDefinition*)layersGetLayer(_textures.layers, layersAddLayer(_textures.layers, NULL));
        texture->material.base = COLOR_WHITE;
        texture->material.reflection = 0.3;
        texture->material.shininess = 2.0;
        texture->bump_height = 0.0;
        texturesLayerValidateDefinition(texture);
    }

    return result;
}

Color terrainGetPreviewColor(Renderer* renderer, double x, double z, double detail)
{
    Vector3 point;

    point.x = x;
    point.y = renderer->terrain->getHeight(renderer, x, z, 1);
    point.z = z;

    return renderer->terrain->getFinalColor(renderer, point, detail);
}
