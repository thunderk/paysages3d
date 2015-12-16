#include "WaterDefinition.h"

#include "PackStream.h"
#include "NoiseState.h"
#include "SurfaceMaterial.h"
#include "IntNode.h"
#include "FloatNode.h"
#include "ColorNode.h"

WaterDefinition::WaterDefinition(DefinitionNode *parent) : DefinitionNode(parent, "water", "water") {
    model = new IntNode(this, "model", -1);
    reflection = new FloatNode(this, "reflection");
    xoffset = new FloatNode(this, "xoffset");
    zoffset = new FloatNode(this, "zoffset");
    depth_color = new ColorNode(this, "depth_color");

    material = new SurfaceMaterial;
    foam_material = new SurfaceMaterial;
    noise_state = new NoiseState();

    transparency_depth = 0.0;
    transparency = 0.0;
    lighting_depth = 0.0;
    scaling = 1.0;
    waves_height = 0.0;
    detail_height = 0.0;
    turbulence = 0.0;
    foam_coverage = 0.0;

    model->addWatcher(this, true);
}

WaterDefinition::~WaterDefinition() {
    delete material;
    delete foam_material;
    delete noise_state;
}

void WaterDefinition::save(PackStream *stream) const {
    DefinitionNode::save(stream);

    material->save(stream);
    stream->write(&transparency_depth);
    stream->write(&transparency);
    stream->write(&lighting_depth);

    stream->write(&scaling);
    stream->write(&waves_height);
    stream->write(&detail_height);
    stream->write(&turbulence);

    stream->write(&foam_coverage);
    foam_material->save(stream);

    noise_state->save(stream);
}

void WaterDefinition::load(PackStream *stream) {
    DefinitionNode::load(stream);

    material->load(stream);
    stream->read(&transparency_depth);
    stream->read(&transparency);
    stream->read(&lighting_depth);

    stream->read(&scaling);
    stream->read(&waves_height);
    stream->read(&detail_height);
    stream->read(&turbulence);

    stream->read(&foam_coverage);
    foam_material->load(stream);

    noise_state->load(stream);

    validate();
}

void WaterDefinition::copy(DefinitionNode *_destination) const {
    DefinitionNode::copy(_destination);

    WaterDefinition *destination = (WaterDefinition *)_destination;
    *destination->material = *material;
    destination->transparency_depth = transparency_depth;
    destination->transparency = transparency;
    destination->lighting_depth = lighting_depth;
    destination->scaling = scaling;
    destination->waves_height = waves_height;
    destination->detail_height = detail_height;
    destination->turbulence = turbulence;
    destination->foam_coverage = foam_coverage;
    *destination->foam_material = *foam_material;
    noise_state->copy(destination->noise_state);
}

void WaterDefinition::validate() {
    DefinitionNode::validate();

    material->base->a = 1.0;
    material->reflection = 1.0;
    material->shininess = 16.0;
    material->hardness = 0.3;
    foam_material->setColor(0.8, 0.8, 0.8, 1.0);
    foam_material->reflection = 0.1;
    foam_material->shininess = 1.5;
    foam_material->hardness = 0.2;

    material->validate();
    foam_material->validate();
}

void WaterDefinition::nodeChanged(const DefinitionNode *node, const DefinitionDiff *) {
    if (node == model) {
        switch (model->getValue()) {
        case 1:
            transparency = 0.3;
            reflection->setValue(0.07);
            transparency_depth = 3.0;
            material->setColor(0.05, 0.18, 0.2, 1.0);
            depth_color->setValue(Color(0.0, 0.18, 0.15));
            lighting_depth = 4.0;
            scaling = 1.5;
            waves_height = 1.0;
            detail_height = 0.06;
            turbulence = 0.3;
            foam_coverage = 0.4;
            break;
        case 0:
        default:
            transparency = 0.5;
            reflection->setValue(0.2);
            transparency_depth = 4.0;
            material->setColor(0.08, 0.15, 0.2, 1.0);
            depth_color->setValue(Color(0.0, 0.1, 0.1));
            lighting_depth = 6.0;
            scaling = 1.0;
            waves_height = 0.8;
            detail_height = 0.05;
            turbulence = 0.1;
            foam_coverage = 0.15;
            break;
        }
    }
}

void WaterDefinition::applyPreset(WaterPreset preset, RandomGenerator &random) {
    noise_state->randomizeOffsets(random);

    if (preset == WATER_PRESET_LAKE) {
        model->setValue(0);
    } else if (preset == WATER_PRESET_SEA) {
        model->setValue(1);
    }
}
