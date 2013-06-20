#include "private.h"

#include <assert.h>
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
typedef struct
{
    double light_power;
    double out_scattering; /* Amount of light scattered away by heavy particles */
} AccumulatedLightData;

static void _walkerFilterCallback(CloudsWalker* walker)
{
    CloudWalkerStepInfo* segment = cloudsWalkerGetLastSegment(walker);
    AccumulatedLightData* data = (AccumulatedLightData*)segment->data;

    assert(data != NULL);

    double density_integral = segment->length * (segment->start.global_density + segment->end.global_density) / 2.0;

    data->out_scattering += 0.3 * density_integral;

    if (data->out_scattering > data->light_power)
    {
        cloudsWalkerOrderStop(walker);
    }
}

static int _alterLight(Renderer* renderer, LightDefinition* light, Vector3 location)
{
    CloudsDefinition* definition = renderer->clouds->definition;
    int i, n;

    AccumulatedLightData data;
    data.out_scattering = 0.0;
    data.light_power = colorGetPower(&light->color);

    /* TODO Iter layers in sorted order */
    n = layersCount(definition->layers);
    for (i = 0; i < n; i++)
    {
        CloudsLayerDefinition* layer = (CloudsLayerDefinition*)layersGetLayer(renderer->clouds->definition->layers, i);
        Vector3 ostart, oend;

        ostart = location;
        oend = v3Add(location, v3Scale(light->direction, -10000.0));
        if (!cloudsOptimizeWalkingBounds(layer, &ostart, &oend))
        {
            continue;
        }
        else
        {
            CloudsWalker* walker;

            walker = cloudsCreateWalker(renderer, layer, ostart, oend);
            cloudsWalkerSetStepSize(walker, -1.0);
            cloudsStartWalking(walker, _walkerFilterCallback, &data);
            cloudsDeleteWalker(walker);
        }
    }

    double max_power = colorGetPower(&light->color) - data.out_scattering;
    if (max_power < 0.0)
    {
        light->color = COLOR_BLACK;
    }
    else
    {
        colorLimitPower(&light->color, max_power);
    }

    return data.out_scattering > 0.0;
}

typedef struct
{
    double out_scattering; /* Amount of light scattered away by heavy particles */
    Color in_scattering; /* Amount of light redirected toward the viewer */
} AccumulatedMaterialData;

static inline void _applyOutScattering(Color* col, double out_scattering)
{
    if (out_scattering >= 1.0)
    {
        col->r = col->g = col->b = 0.0;
    }
    else
    {
        col->r *= (1.0 - out_scattering);
        col->g *= (1.0 - out_scattering);
        col->b *= (1.0 - out_scattering);
    }
}

static void _walkerMaterialCallback(CloudsWalker* walker)
{
    CloudWalkerStepInfo* segment = cloudsWalkerGetLastSegment(walker);
    AccumulatedMaterialData* data = (AccumulatedMaterialData*)segment->data;
    Renderer* renderer = segment->renderer;
    CloudsLayerDefinition* layer = segment->layer;

    assert(data != NULL);

    double density_integral = segment->length * (segment->start.global_density + segment->end.global_density) / 2.0;

    data->out_scattering += 0.5 * density_integral;

    Color in_scattering = renderer->applyLightingToSurface(renderer, segment->start.location, VECTOR_ZERO, &layer->material);
    in_scattering.r *= density_integral * 5.0;
    in_scattering.g *= density_integral * 5.0;
    in_scattering.b *= density_integral * 5.0;
    _applyOutScattering(&in_scattering, data->out_scattering);

    data->in_scattering.r += in_scattering.r;
    data->in_scattering.g += in_scattering.g;
    data->in_scattering.b += in_scattering.b;
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
            data.out_scattering = 0.0;
            data.in_scattering = COLOR_BLACK;

            walker = cloudsCreateWalker(renderer, layer, ostart, oend);
            cloudsWalkerSetStepSize(walker, -1.0);
            cloudsStartWalking(walker, _walkerMaterialCallback, &data);
            cloudsDeleteWalker(walker);

            /* Apply final out_scattering to base */
            _applyOutScattering(&base, data.out_scattering);

            /* Apply in_scattering */
            base.r += data.in_scattering.r;
            base.g += data.in_scattering.g;
            base.b += data.in_scattering.b;

            /* Apply aerial perspective approximation */
            /* TODO This should be done at cloud entry */
            base = renderer->atmosphere->applyAerialPerspective(renderer, ostart, base).final;
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
