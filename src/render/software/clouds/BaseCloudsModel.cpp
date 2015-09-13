#include "BaseCloudsModel.h"

#include "CloudLayerDefinition.h"

BaseCloudsModel::BaseCloudsModel(CloudLayerDefinition *layer):
    layer(layer)
{
}

BaseCloudsModel::~BaseCloudsModel()
{
}

void BaseCloudsModel::update()
{
}

void BaseCloudsModel::getAltitudeRange(double *min_altitude, double *max_altitude) const
{
    *min_altitude = layer->altitude;
    *max_altitude = layer->altitude * layer->scaling;
}

void BaseCloudsModel::getDetailRange(double *min_step, double *max_step) const
{
    double min_altitude, max_altitude, thickness;
    getAltitudeRange(&min_altitude, &max_altitude);
    thickness = max_altitude - min_altitude;
    *min_step = thickness * 0.001;
    *max_step = thickness * 0.2;
}

double BaseCloudsModel::getProbability(const Vector3 &, double) const
{
    return 1.0;
}

double BaseCloudsModel::getDensity(const Vector3 &) const
{
    return 0.0;
}

Color BaseCloudsModel::filterLight(const Color &light, double, double) const
{
    return light;
}

Color BaseCloudsModel::applyLightExit(const Color &light, const Vector3 &, const Vector3 &) const
{
    return light;
}

