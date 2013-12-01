#include "BaseCloudLayerRenderer.h"

#include "CloudLayerDefinition.h"

BaseCloudLayerRenderer::BaseCloudLayerRenderer(SoftwareRenderer* parent):
    parent(parent)
{
}

BaseCloudLayerRenderer::~BaseCloudLayerRenderer()
{

}

double BaseCloudLayerRenderer::getDensity(CloudLayerDefinition *, const Vector3 &)
{
    return 0.0;
}

Color BaseCloudLayerRenderer::getColor(CloudLayerDefinition *, const Vector3 &, const Vector3 &)
{
    return COLOR_TRANSPARENT;
}

bool BaseCloudLayerRenderer::alterLight(CloudLayerDefinition *, LightDefinition *, const Vector3 &, const Vector3 &)
{
    return false;
}

bool BaseCloudLayerRenderer::optimizeSearchLimits(CloudLayerDefinition *layer, Vector3 *start, Vector3 *end)
{
    Vector3 diff;

    if (start->y > layer->lower_altitude + layer->thickness)
    {
        if (end->y >= layer->lower_altitude + layer->thickness)
        {
            return false;
        }
        else
        {
            diff = v3Sub(*end, *start);
            *start = v3Add(*start, v3Scale(diff, (layer->lower_altitude + layer->thickness - start->y) / diff.y));
            if (end->y < layer->lower_altitude)
            {
                *end = v3Add(*end, v3Scale(diff, (layer->lower_altitude - end->y) / diff.y));
            }
        }
    }
    else if (start->y < layer->lower_altitude)
    {
        if (end->y <= layer->lower_altitude)
        {
            return false;
        }
        else
        {
            diff = v3Sub(*end, *start);
            *start = v3Add(*start, v3Scale(diff, (layer->lower_altitude - start->y) / diff.y));
            if (end->y >= layer->lower_altitude + layer->thickness)
            {
                *end = v3Add(*end, v3Scale(diff, (layer->lower_altitude + layer->thickness - end->y) / diff.y));
            }
        }
    }
    else /* start is inside layer */
    {
        diff = v3Sub(*end, *start);
        if (end->y > layer->lower_altitude + layer->thickness)
        {
            *end = v3Add(*start, v3Scale(diff, (layer->lower_altitude + layer->thickness - start->y) / diff.y));
        }
        else if (end->y < layer->lower_altitude)
        {
            *end = v3Add(*start, v3Scale(diff, (layer->lower_altitude - start->y) / diff.y));
        }
    }

    return true;
}
