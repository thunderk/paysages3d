#include "TerrainDefinition.h"

#include "FloatNode.h"
#include "FractalNoise.h"
#include "NoiseGenerator.h"
#include "NoiseNode.h"
#include "PackStream.h"
#include "TerrainHeightMap.h"
#include <cmath>

TerrainDefinition::TerrainDefinition(DefinitionNode *parent) : DefinitionNode(parent, "terrain", "terrain") {
    shadow_smoothing = 0.0;

    height_map = new TerrainHeightMap(this);
    has_painting = false;
    addChild(height_map);

    water_height = new FloatNode(this, "water_height");
    height_noise = new NoiseNode(this, "height_noise");
}

TerrainDefinition::~TerrainDefinition() {
}

void TerrainDefinition::validate() {
    DefinitionNode::validate();

    // Get base noise range
    height_noise->getGenerator()->estimateRange(&_min_height, &_max_height, 0.1);
    // TODO Alter limits with heightmap min/max, and displacement textures

    has_painting = height_map->hasPainting();
}

void TerrainDefinition::copy(DefinitionNode *_destination) const {
    if (auto destination = dynamic_cast<TerrainDefinition *>(_destination)) {
        destination->shadow_smoothing = shadow_smoothing;

        height_map->copy(destination->height_map);

        destination->validate();
    }
}

void TerrainDefinition::save(PackStream *stream) const {
    DefinitionNode::save(stream);

    stream->write(&shadow_smoothing);
}

void TerrainDefinition::load(PackStream *stream) {
    DefinitionNode::load(stream);

    stream->read(&shadow_smoothing);

    validate();
}

double TerrainDefinition::getGridHeight(int x, int z, bool with_painting) {
    double h;

    if (!with_painting || !has_painting || !height_map->getGridValue(x, z, &h)) {
        h = height_noise->getGenerator()->get2d(0.1, to_double(x), to_double(z));
    }

    return h;
}

double TerrainDefinition::getInterpolatedHeight(double x, double z, bool with_painting, bool water_offset) {
    double h;

    if (!with_painting || !has_painting || !height_map->getInterpolatedValue(x, z, &h)) {
        h = height_noise->getGenerator()->get2d(0.1, x, z);
    }

    return (water_offset ? (h + getWaterOffset()) : h);
}

double TerrainDefinition::getWaterOffset() const {
    double height_power = (_max_height - _min_height) * 0.5;
    return -water_height->getValue() * height_power;
}

HeightInfo TerrainDefinition::getHeightInfo() {
    HeightInfo result;

    result.min_height = _min_height;
    result.max_height = _max_height;
    result.base_height = -getWaterOffset();

    return result;
}

unsigned long TerrainDefinition::getMemoryStats() {
    return height_map->getMemoryStats();
}

void TerrainDefinition::applyPreset(TerrainPreset preset, RandomGenerator &random) {
    switch (preset) {
    case TERRAIN_PRESET_STANDARD:
        height_noise->randomize(random);
        height_noise->setConfig(400.0, 0.1, 0.5, 1.02);
        shadow_smoothing = 0.03;
        break;
    }

    water_height->setValue(-0.3);

    height_map->clearPainting();
    validate();
}
