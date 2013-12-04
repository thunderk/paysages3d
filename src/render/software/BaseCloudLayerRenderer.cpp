#include "BaseCloudLayerRenderer.h"

#include "clouds/BaseCloudsModel.h"

BaseCloudLayerRenderer::BaseCloudLayerRenderer(SoftwareRenderer* parent):
    parent(parent)
{
}

BaseCloudLayerRenderer::~BaseCloudLayerRenderer()
{

}

Color BaseCloudLayerRenderer::getColor(BaseCloudsModel *, const Vector3 &, const Vector3 &)
{
    return COLOR_TRANSPARENT;
}

bool BaseCloudLayerRenderer::alterLight(BaseCloudsModel *, LightDefinition *, const Vector3 &, const Vector3 &)
{
    return false;
}

bool BaseCloudLayerRenderer::optimizeSearchLimits(BaseCloudsModel *model, Vector3 *start, Vector3 *end)
{
    Vector3 diff;
    double min_altitude, max_altitude;

    model->getAltitudeRange(&min_altitude, &max_altitude);

    if (start->y > max_altitude)
    {
        if (end->y >= max_altitude)
        {
            return false;
        }
        else
        {
            diff = v3Sub(*end, *start);
            *start = v3Add(*start, v3Scale(diff, (max_altitude - start->y) / diff.y));
            if (end->y < min_altitude)
            {
                *end = v3Add(*end, v3Scale(diff, (min_altitude - end->y) / diff.y));
            }
        }
    }
    else if (start->y < min_altitude)
    {
        if (end->y <= min_altitude)
        {
            return false;
        }
        else
        {
            diff = v3Sub(*end, *start);
            *start = v3Add(*start, v3Scale(diff, (min_altitude - start->y) / diff.y));
            if (end->y >= max_altitude)
            {
                *end = v3Add(*end, v3Scale(diff, (max_altitude - end->y) / diff.y));
            }
        }
    }
    else /* start is inside layer */
    {
        diff = v3Sub(*end, *start);
        if (end->y > max_altitude)
        {
            *end = v3Add(*start, v3Scale(diff, (max_altitude - start->y) / diff.y));
        }
        else if (end->y < min_altitude)
        {
            *end = v3Add(*start, v3Scale(diff, (min_altitude - start->y) / diff.y));
        }
    }

    return true;
}
