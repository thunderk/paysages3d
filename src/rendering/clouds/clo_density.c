#include "clo_density.h"

#include "../tools.h"

double cloudsGetLayerCoverage(CloudsLayerDefinition* layer, Vector3 location)
{
    if (layer->base_coverage == 0.0)
    {
        return 0.0;
    }
    else
    {
        double coverage = noiseGet2DTotal(layer->_coverage_noise, location.x / layer->shape_scaling, location.z / layer->shape_scaling);
        coverage -= (1.0 - layer->base_coverage);

        coverage *= curveGetValue(layer->_coverage_by_altitude, (location.y - layer->lower_altitude) / layer->thickness);

        return (coverage <= 0.0) ? 0.0 : coverage;
    }
}

double cloudsGetLayerDensity(CloudsLayerDefinition* layer, Vector3 location, double coverage)
{
    if (coverage == 0.0)
    {
        return 0.0;
    }
    else if (coverage == 1.0)
    {
        return 1.0;
    }
    else
    {
        double density = noiseGet3DTotal(layer->_shape_noise, location.x / layer->shape_scaling, location.y / layer->shape_scaling, location.z / layer->shape_scaling);
        density -= (0.5 - coverage);
        return (density <= 0.0) ? 0.0 : density;
    }
}

static double _fakeGetDensity(Renderer* renderer, CloudsLayerDefinition* layer, Vector3 location)
{
    UNUSED(layer);
    UNUSED(renderer);
    UNUSED(location);

    return 0.0;
}

void cloudsBindFakeDensityToRenderer(CloudsRenderer* renderer)
{
    renderer->getLayerDensity = _fakeGetDensity;
}

static double _realGetDensity(Renderer* renderer, CloudsLayerDefinition* layer, Vector3 location)
{
    UNUSED(renderer);

    double coverage = cloudsGetLayerCoverage(layer, location);

    return cloudsGetLayerDensity(layer, location, coverage);
}

void cloudsBindRealDensityToRenderer(CloudsRenderer* renderer)
{
    renderer->getLayerDensity = _realGetDensity;
}
