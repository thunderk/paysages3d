#include "CloudLayerDefinition.h"

#include "Curve.h"
#include "FloatNode.h"
#include "NoiseGenerator.h"
#include "PackStream.h"
#include "SurfaceMaterial.h"

CloudLayerDefinition::CloudLayerDefinition(DefinitionNode *parent, const string &name)
    : DefinitionNode(parent, name, "cloudlayer") {
    type = CIRRUS;
    altitude = 0.5;
    scaling = 0.5;
    coverage = 0.5;

    xoffset = new FloatNode(this, "xoffset");
    zoffset = new FloatNode(this, "zoffset");
}

CloudLayerDefinition::~CloudLayerDefinition() {
}

CloudLayerDefinition *CloudLayerDefinition::newCopy(const CloudLayerDefinition &other, DefinitionNode *parent) {
    CloudLayerDefinition *layer = new CloudLayerDefinition(parent, other.getName());
    other.copy(layer);
    return layer;
}

CloudLayerDefinition *CloudLayerDefinition::newCopy(DefinitionNode *parent) const {
    CloudLayerDefinition *layer = new CloudLayerDefinition(parent, getName());
    copy(layer);
    return layer;
}

void CloudLayerDefinition::save(PackStream *stream) const {
    DefinitionNode::save(stream);

    int clouds_type = static_cast<int>(type);

    stream->write(&clouds_type);
    stream->write(&altitude);
    stream->write(&scaling);
    stream->write(&coverage);

    noise_state.save(stream);
}

void CloudLayerDefinition::load(PackStream *stream) {
    DefinitionNode::load(stream);

    int clouds_type;

    stream->read(&clouds_type);
    type = static_cast<CloudsType>(clouds_type);
    stream->read(&altitude);
    stream->read(&scaling);
    stream->read(&coverage);

    noise_state.load(stream);

    validate();
}

void CloudLayerDefinition::copy(DefinitionNode *_destination) const {
    DefinitionNode::copy(_destination);

    CloudLayerDefinition *destination = (CloudLayerDefinition *)_destination;

    destination->type = type;
    destination->altitude = altitude;
    destination->scaling = scaling;
    destination->coverage = coverage;

    noise_state.copy(&destination->noise_state);
}

void CloudLayerDefinition::validate() {
    DefinitionNode::validate();

    if (scaling < 0.1) {
        scaling = 0.1;
    }
}
