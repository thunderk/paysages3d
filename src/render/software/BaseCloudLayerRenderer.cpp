#include "BaseCloudLayerRenderer.h"

#include "clouds/BaseCloudsModel.h"
#include "Vector3.h"

BaseCloudLayerRenderer::BaseCloudLayerRenderer(SoftwareRenderer *parent) : parent(parent) {
    setQuality(0.5);
}

BaseCloudLayerRenderer::~BaseCloudLayerRenderer() {
}

Color BaseCloudLayerRenderer::getColor(BaseCloudsModel *, const Vector3 &, const Vector3 &) {
    return COLOR_TRANSPARENT;
}

bool BaseCloudLayerRenderer::alterLight(BaseCloudsModel *, LightComponent *, const Vector3 &, const Vector3 &) {
    return false;
}

void BaseCloudLayerRenderer::setQuality(double quality) {
    this->quality = quality;
}

bool BaseCloudLayerRenderer::optimizeSearchLimits(BaseCloudsModel *model, Vector3 *start, Vector3 *end) {
    Vector3 diff;
    double min_altitude, max_altitude;

    model->getAltitudeRange(&min_altitude, &max_altitude);

    if (start->y > max_altitude) {
        if (end->y >= max_altitude) {
            return false;
        } else {
            diff = end->sub(*start);
            *start = start->add(diff.scale((max_altitude - start->y) / diff.y));
            if (end->y < min_altitude) {
                *end = end->add(diff.scale((min_altitude - end->y) / diff.y));
            }
        }
    } else if (start->y < min_altitude) {
        if (end->y <= min_altitude) {
            return false;
        } else {
            diff = end->sub(*start);
            *start = start->add(diff.scale((min_altitude - start->y) / diff.y));
            if (end->y >= max_altitude) {
                *end = end->add(diff.scale((max_altitude - end->y) / diff.y));
            }
        }
    } else /* start is inside layer */
    {
        diff = end->sub(*start);
        if (end->y > max_altitude) {
            *end = start->add(diff.scale((max_altitude - start->y) / diff.y));
        } else if (end->y < min_altitude) {
            *end = start->add(diff.scale((min_altitude - start->y) / diff.y));
        }
    }

    return true;
}
