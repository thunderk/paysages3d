#include "NoiseNode.h"

#include "NoiseGenerator.h"
#include "Logs.h"

NoiseNode::NoiseNode(DefinitionNode *parent) : DefinitionNode(parent, "noise") {
    noise = new NoiseGenerator();
}

NoiseNode::~NoiseNode() {
    delete noise;
}

void NoiseNode::setLevels(int levels, double min_value, double max_value) {
    noise->clearLevels();
    noise->addLevelsSimple(levels, 1.0, -1.0, 1.0, 0.5);
    noise->normalizeAmplitude(min_value, max_value, false);
    noise->randomizeOffsets();
    noise->validate();
}

void NoiseNode::save(PackStream *stream) const {
    noise->save(stream);
}

void NoiseNode::load(PackStream *stream) {
    noise->load(stream);
}

void NoiseNode::copy(DefinitionNode *destination) const {
    if (destination->getTypeName() == getTypeName()) {
        noise->copy(((NoiseNode *)destination)->noise);
    } else {
        Logs::error("Definition") << "Can't copy from " << getTypeName() << " to " << destination->getTypeName()
                                  << endl;
    }
}

void NoiseNode::validate() {
    noise->validate();
}
