#include "private.h"

#include <stdlib.h>
#include "../tools.h"
#include "../renderer.h"

/******************** Fake ********************/
static TerrainResult _fakeDisplaceTerrain(Renderer* renderer, TerrainResult terrain)
{
    UNUSED(renderer);

    return terrain;
}

static TexturesResult _fakeApplyToTerrain(Renderer* renderer, TerrainResult terrain)
{
    TexturesResult result;

    result.base_location = terrain.location;
    result.base_normal = terrain.normal;
    result.layer_count = 0;
    result.final_location = terrain.location;
    result.final_color = COLOR_WHITE;

    return result;
}

/******************** Real ********************/
static TerrainResult _realDisplaceTerrain(Renderer* renderer, TerrainResult terrain)
{
    /* TODO */
    return _fakeDisplaceTerrain(renderer, terrain);
}

static TexturesResult _realApplyToTerrain(Renderer* renderer, TerrainResult terrain)
{
    /* TODO */
    return _fakeApplyToTerrain(renderer, terrain);
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
