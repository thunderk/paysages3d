#include "TerrainDefinition.h"

#include <cmath>
#include "TerrainHeightMap.h"
#include "NoiseGenerator.h"
#include "PackStream.h"
#include "FloatNode.h"

TerrainDefinition::TerrainDefinition(DefinitionNode *parent) : DefinitionNode(parent, "terrain", "terrain") {
    height = 1.0;
    shadow_smoothing = 0.0;

    height_map = new TerrainHeightMap(this);
    has_painting = false;
    addChild(height_map);

    water_height = new FloatNode(this, "water_height");

    _height_noise = new NoiseGenerator;
}

TerrainDefinition::~TerrainDefinition() {
    delete _height_noise;
}

void TerrainDefinition::validate() {
    _height_noise->validate();

    if (height < 1.0) {
        height = 1.0;
    }

    /* Get minimal and maximal height */
    _height_noise->getRange(&_min_height, &_max_height);
    _min_height *= height;
    _max_height *= height;

    /* TODO Alter limits with heightmap min/max */
    has_painting = height_map->hasPainting();
}

void TerrainDefinition::copy(DefinitionNode *_destination) const {
    TerrainDefinition *destination = (TerrainDefinition *)_destination;

    destination->height = height;
    destination->shadow_smoothing = shadow_smoothing;

    height_map->copy(destination->height_map);

    _height_noise->copy(destination->_height_noise);

    destination->validate();
}

void TerrainDefinition::save(PackStream *stream) const {
    DefinitionNode::save(stream);

    stream->write(&height);
    stream->write(&shadow_smoothing);
    _height_noise->save(stream);
}

void TerrainDefinition::load(PackStream *stream) {
    DefinitionNode::load(stream);

    stream->read(&height);
    stream->read(&shadow_smoothing);
    _height_noise->load(stream);

    validate();
}

double TerrainDefinition::getGridHeight(int x, int z, bool with_painting) {
    double h;

    if (!with_painting || !has_painting || !height_map->getGridValue(x, z, &h)) {
        h = _height_noise->get2DTotal((double)x, (double)z);
    }

    return h;
}

double TerrainDefinition::getInterpolatedHeight(double x, double z, bool scaled, bool with_painting,
                                                bool water_offset) {
    double h;

    if (!with_painting || !has_painting || !height_map->getInterpolatedValue(x, z, &h)) {
        h = _height_noise->get2DTotal(x, z);
    }

    if (scaled) {
        return (water_offset ? (h - water_height->getValue()) : h) * height;
    } else {
        return h;
    }
}

double TerrainDefinition::getWaterOffset() const {
    return -water_height->getValue() * height;
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

void TerrainDefinition::applyPreset(TerrainPreset preset) {
    int resolution = 8;
    switch (preset) {
    case TERRAIN_PRESET_STANDARD:
        _height_noise->randomizeOffsets();
        _height_noise->clearLevels();
        _height_noise->addLevelSimple(pow(2.0, resolution + 1), -1.0, 1.0);
        _height_noise->addLevelsSimple(resolution - 2, pow(2.0, resolution - 1), -0.7, 0.7, 0.5);
        _height_noise->normalizeAmplitude(-1.0, 1.0, 0);
        _height_noise->setFunctionParams(NoiseGenerator::NOISE_FUNCTION_SIMPLEX, 0.0, 0.0);
        height = 30.0;
        shadow_smoothing = 0.03;
        break;
    default:
        ;
    }

    water_height->setValue(-0.3);

    height_map->clearPainting();
    validate();
}
