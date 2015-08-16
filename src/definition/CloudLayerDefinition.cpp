#include "CloudLayerDefinition.h"

#include "Curve.h"
#include "NoiseGenerator.h"
#include "SurfaceMaterial.h"
#include "PackStream.h"

CloudLayerDefinition::CloudLayerDefinition(DefinitionNode* parent):
    DefinitionNode(parent, "layer", "cloudlayer")
{
    type = CIRRUS;
    altitude = 0.5;
    scaling = 0.5;
    coverage = 0.5;
}

CloudLayerDefinition::~CloudLayerDefinition()
{
}

CloudLayerDefinition* CloudLayerDefinition::newCopy(const CloudLayerDefinition& other, DefinitionNode* parent)
{
    CloudLayerDefinition* layer = new CloudLayerDefinition(parent);
    other.copy(layer);
    return layer;
}

CloudLayerDefinition* CloudLayerDefinition::newCopy(DefinitionNode* parent) const
{
    CloudLayerDefinition* layer = new CloudLayerDefinition(parent);
    copy(layer);
    return layer;
}

void CloudLayerDefinition::save(PackStream* stream) const
{
    DefinitionNode::save(stream);

    int clouds_type = (int)type;

    stream->write(&clouds_type);
    stream->write(&altitude);
    stream->write(&scaling);
    stream->write(&coverage);

    noise_state.save(stream);
}

void CloudLayerDefinition::load(PackStream* stream)
{
    DefinitionNode::load(stream);

    int clouds_type;

    stream->read(&clouds_type);
    type = (CloudsType)clouds_type;
    stream->read(&altitude);
    stream->read(&scaling);
    stream->read(&coverage);

    noise_state.load(stream);

    validate();
}

void CloudLayerDefinition::copy(DefinitionNode* _destination) const
{
    DefinitionNode::copy(_destination);

    CloudLayerDefinition* destination = (CloudLayerDefinition*)_destination;

    destination->type = type;
    destination->altitude = altitude;
    destination->scaling = scaling;
    destination->coverage = coverage;

    noise_state.copy(&destination->noise_state);
}

void CloudLayerDefinition::validate()
{
    if (scaling < 0.1)
    {
        scaling = 0.1;
    }
}
