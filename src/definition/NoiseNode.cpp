#include "NoiseNode.h"

#include "NoiseFunctionSimplex.h"
#include "Logs.h"

NoiseNode::NoiseNode(DefinitionNode *parent, const string &name) : DefinitionNode(parent, name) {
    noise = new NoiseFunctionSimplex();
}

NoiseNode::~NoiseNode() {
    delete noise;
}

void NoiseNode::randomize(RandomGenerator &random) {
    noise->randomize(random);
}

void NoiseNode::setConfig(double scaling, double height, double step_scaling, double step_height) {
    noise->setScaling(scaling, height);
    noise->setStep(step_scaling, step_height);
}

void NoiseNode::forceSetGenerator(FractalNoise *noise) {
    delete this->noise;
    this->noise = noise;
}

void NoiseNode::save(PackStream *stream) const {
    noise->save(stream);
}

void NoiseNode::load(PackStream *stream) {
    noise->load(stream);
}

void NoiseNode::copy(DefinitionNode *destination) const {
    if (destination->getTypeName() == getTypeName()) {
        if (auto tdestination = dynamic_cast<NoiseNode *>(destination)) {
            noise->copy(tdestination->noise);
        }
    } else {
        Logs::error("Definition") << "Can't copy from " << getTypeName() << " to " << destination->getTypeName()
                                  << endl;
    }
}

string NoiseNode::toString(int indent) const {
    return DefinitionNode::toString(indent) + " - scaling: " + to_string(noise->getScaling()) + " step " +
           to_string(noise->getStepScaling()) + " - height: " + to_string(noise->getHeight()) + " step " +
           to_string(noise->getStepHeight());
}
