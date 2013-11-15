#include "private.h"

#include <assert.h>
#include <stdlib.h>
#include "../tools.h"
#include "../renderer.h"
#include "clo_density.h"
#include "clo_walking.h"
#include "atmosphere/public.h"
#include "CloudsDefinition.h"
#include "CloudLayerDefinition.h"

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
    Renderer* renderer = segment->renderer;
    AccumulatedLightData* data = (AccumulatedLightData*)segment->data;

    assert(data != NULL);

    double density_integral;
    if (segment->subdivided)
    {
        density_integral = segment->length * (segment->start.local_density + segment->end.local_density) / 2.0;
    }
    else
    {
        if (!segment->refined && segment->start.global_density == 0.0 && segment->end.global_density > 0.0)
        {
            cloudsWalkerOrderRefine(walker, 1.0 / (double)renderer->render_quality);
            return;
        }
        else if ((segment->start.global_density > 0.0 || segment->end.global_density > 0.0) && (segment->start.global_density < 1.0 || segment->end.global_density < 1.0))
        {
            cloudsWalkerOrderSubdivide(walker, renderer->render_quality + 1);
            return;
        }
        density_integral = segment->length * (segment->start.global_density + segment->end.global_density) / 2.0;
    }

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
    n = definition->count();
    for (i = 0; i < n; i++)
    {
        CloudLayerDefinition* layer = definition->getCloudLayer(i);
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
            cloudsWalkerSetVoidSkipping(walker, 1);
            cloudsStartWalking(walker, _walkerFilterCallback, &data);
            cloudsDeleteWalker(walker);
        }
    }

    double max_power = data.light_power - data.out_scattering;
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
    int entry_found; /* 1 if a cloud entry has been found */
    Vector3 entry; /* Point of entry in the clouds */
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
    CloudLayerDefinition* layer = segment->layer;

    assert(data != NULL);

    double density_integral;
    if (segment->subdivided)
    {
        density_integral = segment->length * (segment->start.local_density + segment->end.local_density) / 2.0;
    }
    else
    {
        if (!segment->refined && segment->start.global_density == 0.0 && segment->end.global_density > 0.0)
        {
            cloudsWalkerOrderRefine(walker, 1.0 / (double)(renderer->render_quality * renderer->render_quality));
            return;
        }
        else if ((segment->start.global_density > 0.0 || segment->end.global_density > 0.0) && (segment->start.global_density < 1.0 || segment->end.global_density < 1.0))
        {
            cloudsWalkerOrderSubdivide(walker, renderer->render_quality + 3);
            return;
        }
        density_integral = segment->length * (segment->start.global_density + segment->end.global_density) / 2.0;
    }

    if (density_integral > 0.0)
    {
        data->out_scattering += 0.3 * density_integral;

        Color in_scattering = renderer->applyLightingToSurface(renderer, segment->start.location, VECTOR_ZERO, layer->material);
        in_scattering.r *= density_integral * 5.0;
        in_scattering.g *= density_integral * 5.0;
        in_scattering.b *= density_integral * 5.0;
        _applyOutScattering(&in_scattering, data->out_scattering);

        data->in_scattering.r += in_scattering.r;
        data->in_scattering.g += in_scattering.g;
        data->in_scattering.b += in_scattering.b;

        if (!data->entry_found)
        {
            data->entry_found = 1;
            data->entry = segment->start.location;
        }
    }
}

static Color _getColor(Renderer* renderer, Color base, Vector3 start, Vector3 end)
{
    CloudsDefinition* definition = renderer->clouds->definition;
    int i, n;

    n = definition->count();
    if (n < 1)
    {
        return base;
    }

    /* TODO Iter layers in sorted order */
    for (i = 0; i < n; i++)
    {
        CloudLayerDefinition* layer = definition->getCloudLayer(i);
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
            data.entry = ostart;
            data.entry_found = 0;

            walker = cloudsCreateWalker(renderer, layer, ostart, oend);
            cloudsWalkerSetStepSize(walker, -1.0);
            cloudsWalkerSetVoidSkipping(walker, 1);
            cloudsStartWalking(walker, _walkerMaterialCallback, &data);
            cloudsDeleteWalker(walker);

            /* Apply final out_scattering to base */
            _applyOutScattering(&base, data.out_scattering);

            /* Apply in_scattering */
            base.r += data.in_scattering.r;
            base.g += data.in_scattering.g;
            base.b += data.in_scattering.b;

            /* Apply aerial perspective approximation */
            base = renderer->atmosphere->applyAerialPerspective(renderer, data.entry, base).final;
        }
    }

    return base;
}

/******************** Renderer ********************/
static CloudsRenderer* _createRenderer()
{
    CloudsRenderer* result;

    result = new CloudsRenderer;
    result->definition = new CloudsDefinition(NULL);

    result->getColor = _fakeGetColor;
    result->alterLight = (FuncLightingAlterLight)_fakeAlterLight;

    cloudsBindFakeDensityToRenderer(result);

    return result;
}

static void _deleteRenderer(CloudsRenderer* renderer)
{
    delete renderer->definition;
    delete renderer;
}

static void _bindRenderer(Renderer* renderer, CloudsDefinition* definition)
{
    definition->copy(renderer->clouds->definition);

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
