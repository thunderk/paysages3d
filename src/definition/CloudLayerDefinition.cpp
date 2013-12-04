#include "CloudLayerDefinition.h"

#include "Curve.h"
#include "NoiseGenerator.h"
#include "SurfaceMaterial.h"
#include "PackStream.h"

CloudLayerDefinition::CloudLayerDefinition(BaseDefinition* parent):
    BaseDefinition(parent)
{
    type = CIRRUS;
    altitude = 0.5;
    scaling = 0.5;
    coverage = 0.5;
}

CloudLayerDefinition::~CloudLayerDefinition()
{
}

CloudLayerDefinition* CloudLayerDefinition::newCopy(const CloudLayerDefinition& other, BaseDefinition* parent)
{
    CloudLayerDefinition* layer = new CloudLayerDefinition(parent);
    other.copy(layer);
    return layer;
}

CloudLayerDefinition* CloudLayerDefinition::newCopy(BaseDefinition* parent) const
{
    CloudLayerDefinition* layer = new CloudLayerDefinition(parent);
    copy(layer);
    return layer;
}

void CloudLayerDefinition::save(PackStream* stream) const
{
    BaseDefinition::save(stream);

    int clouds_type = (int)type;

    stream->write(&clouds_type);
    stream->write(&altitude);
    stream->write(&scaling);
    stream->write(&coverage);
}

void CloudLayerDefinition::load(PackStream* stream)
{
    BaseDefinition::load(stream);

    int clouds_type;

    stream->read(&clouds_type);
    type = (CloudsType)clouds_type;
    stream->read(&altitude);
    stream->read(&scaling);
    stream->read(&coverage);

    validate();
}

void CloudLayerDefinition::copy(BaseDefinition* _destination) const
{
    BaseDefinition::copy(_destination);

    CloudLayerDefinition* destination = (CloudLayerDefinition*)_destination;

    destination->type = type;
    destination->altitude = altitude;
    destination->scaling = scaling;
    destination->coverage = coverage;
}

void CloudLayerDefinition::validate()
{
    if (scaling < 0.1)
    {
        scaling = 0.1;
    }
}
