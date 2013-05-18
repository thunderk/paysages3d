#include "private.h"

#include <stdlib.h>
#include "../tools.h"
#include "../renderer.h"
#include "clo_density.h"

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
/*static int _cmpLayer(const void* layer1, const void* layer2)
{
    return (((CloudsLayerDefinition*)layer1)->lower_altitude > ((CloudsLayerDefinition*)layer2)->lower_altitude) ? -1 : 1;
}*/

static int _alterLight(Renderer* renderer, LightDefinition* light, Vector3 location)
{
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
        base = cloudsApplyLayer(layersGetLayer(definition->layers, i), base, renderer, start, end);
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
