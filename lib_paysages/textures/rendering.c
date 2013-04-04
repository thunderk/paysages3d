#include "private.h"

#include <stdlib.h>
#include "../tools.h"
#include "../renderer.h"

/******************** Fake ********************/
static Vector3 _fakeDisplaceTerrain(Renderer* renderer, TerrainResult terrain)
{
    UNUSED(renderer);

    return terrain.location;
}

static TexturesResult _fakeApplyToTerrain(Renderer* renderer, double x, double z)
{
    TexturesResult result;

    result.base_location.x = x;
    result.base_location.y = 0.0;
    result.base_location.z = z;
    result.base_normal = VECTOR_UP;
    result.layer_count = 0;
    result.final_location = result.base_location;
    result.final_color = COLOR_WHITE;

    return result;
}

/******************** Real ********************/
static Vector3 _realDisplaceTerrain(Renderer* renderer, TerrainResult terrain)
{
    TexturesDefinition* textures = renderer->textures->definition;
    double offset = 0.0;
    int i, n;

    n = layersCount(textures->layers);
    for (i = 0; i < n; i++)
    {
        TexturesLayerDefinition* layer = layersGetLayer(textures->layers, i);

        if (layer->displacement_height > 0.0)
        {
            double presence = texturesGetLayerBasePresence(layer, terrain);
            Vector3 location = {terrain.location.x / layer->displacement_scaling, terrain.location.y / layer->displacement_scaling, terrain.location.z / layer->displacement_scaling};
            offset += texturesGetTriplanarNoise(layer->_displacement_noise, location, terrain.normal) * presence * layer->displacement_height;
        }
    }

    return v3Add(terrain.location, v3Scale(v3Normalize(terrain.normal), offset));
}

static TexturesResult _realApplyToTerrain(Renderer* renderer, double x, double z)
{
    TexturesResult result;

    /* Displacement */
    TerrainResult terrain = renderer->terrain->getResult(renderer, x, z, 1, 1);

    /* TODO Detail */

    /* TEMP */
    SurfaceMaterial temp;
    temp.base.r = 0.6;
    temp.base.g = 0.55;
    temp.base.b = 0.57;
    temp.reflection = 0.02;
    temp.shininess = 3.0;

    result.base_location = terrain.location;
    result.base_normal = terrain.normal;
    result.final_location = terrain.location;
    result.final_color = renderer->applyLightingToSurface(renderer, terrain.location, terrain.normal, &temp);
    return result;
}

/******************** Renderer ********************/
static TexturesRenderer* _createRenderer()
{
    TexturesRenderer* result;

    result = malloc(sizeof(TexturesRenderer));
    result->definition = TexturesDefinitionClass.create();

    result->displaceTerrain = _fakeDisplaceTerrain;
    result->applyToTerrain = _fakeApplyToTerrain;

    return result;
}

static void _deleteRenderer(TexturesRenderer* renderer)
{
    TexturesDefinitionClass.destroy(renderer->definition);
    free(renderer);
}

static void _bindRenderer(Renderer* renderer, TexturesDefinition* definition)
{
    TexturesDefinitionClass.copy(definition, renderer->textures->definition);

    renderer->textures->displaceTerrain = _realDisplaceTerrain;
    renderer->textures->applyToTerrain = _realApplyToTerrain;
}

StandardRenderer TexturesRendererClass = {
    (FuncObjectCreate)_createRenderer,
    (FuncObjectDelete)_deleteRenderer,
    (FuncObjectBind)_bindRenderer
};
