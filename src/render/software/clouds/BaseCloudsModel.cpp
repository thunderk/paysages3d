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
    *min_step = 0.1;
    *max_step = 1.0;
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

