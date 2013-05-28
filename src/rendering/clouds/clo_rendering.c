#include "private.h"

#include <stdlib.h>
#include "../tools.h"
#include "../renderer.h"
#include "clo_density.h"
#include "clo_walking.h"

/******************** Fake ********************/
static int _fakeAlterLight(Renderer* renderer, LightDefinition* light, Vector3 location)
{
    UNUSED(renderer);
    UNUSED(light);
    UNUSED(location);

    return 0;
}

static Color _fakeGetColor(Renderer* renderer, Color base, Vector3 start, Vector3 end)
{
    UNUSED(renderer);
    UNUSED(start);
    UNUSED(end);

    return base;
}

/******************** Real ********************/
static int _alterLight(Renderer* renderer, LightDefinition* light, Vector3 location)
{
#if 0
    CloudsDefinition* definition = renderer->clouds->definition;
    int i, n;

    /* TODO Iter layers in sorted order */
    n = layersCount(definition->layers);
    for (i = 0; i < n; i++)
    {
        light->color = cloudsLayerFilterLight(layersGetLayer(definition->layers, i), renderer, light->color, location, v3Add(location, v3Scale(light->direction, -10000.0)), v3Scale(light->direction, -1.0));
        /* TODO Reduce light->reflection too */
    }
    return n > 0;
#endif
    return 0;
}

typedef struct
{
    Color result;
} AccumulatedMaterialData;

static void _walkerMaterialCallback(CloudsWalker* walker)
{
    /*AccumulatedMaterialData* data = (AccumulatedMaterialData*)segment->data;*/
}

static Color _getColor(Renderer* renderer, Color base, Vector3 start, Vector3 end)
{
    CloudsDefinition* definition = renderer->clouds->definition;
    int i, n;

    n = layersCount(definition->layers);
    if (n < 1)
    {
        return base;
    }

    /* TODO Iter layers in sorted order */
    for (i = 0; i < n; i++)
    {
        CloudsLayerDefinition* layer = (CloudsLayerDefinition*)layersGetLayer(renderer->clouds->definition->layers, i);
        Vector3 ostart, oend;

        ostart = start;
        oend = end;
        if (!cloudsOptimizeWalkingBounds(layer, &ostart, &oend))
        {
            continue;
        }
        else
        {
            CloudsWalker* walker;
            AccumulatedMaterialData data;
            data.result = COLOR_TRANSPARENT;

            walker = cloudsCreateWalker(renderer, layer, start, end);
            cloudsStartWalking(walker, _walkerMaterialCallback, &data);
            cloudsDeleteWalker(walker);

            colorMask(&base, &data.result);
        }
    }

    return base;
}

/******************** Renderer ********************/
static CloudsRenderer* _createRenderer()
{
    CloudsRenderer* result;

    result = malloc(sizeof(CloudsRenderer));
    result->definition = CloudsDefinitionClass.create();

    result->getColor = _fakeGetColor;
    result->alterLight = (FuncLightingAlterLight)_fakeAlterLight;

    cloudsBindFakeDensityToRenderer(result);

    return result;
}

static void _deleteRenderer(CloudsRenderer* renderer)
{
    CloudsDefinitionClass.destroy(renderer->definition);
    free(renderer);
}

static void _bindRenderer(Renderer* renderer, CloudsDefinition* definition)
{
    CloudsDefinitionClass.copy(definition, renderer->clouds->definition);

    renderer->clouds->getColor = _getColor;
    renderer->clouds->alterLight = (FuncLightingAlterLight)_alterLight;

    cloudsBindRealDensityToRenderer(renderer->clouds);

    lightingManagerRegisterFilter(renderer->lighting, (FuncLightingAlterLight)_alterLight, renderer);
}

StandardRenderer CloudsRendererClass = {
    (FuncObjectCreate)_createRenderer,
    (FuncObjectDelete)_deleteRenderer,
    (FuncObjectBind)_bindRenderer
};
