#include "TerrainDefinition.h"

#include "TerrainHeightMap.h"
#include "NoiseGenerator.h"
#include "PackStream.h"

TerrainDefinition::TerrainDefinition(BaseDefinition* parent):
    BaseDefinition(parent)
{
    height = 1.0;
    scaling = 1.0;
    shadow_smoothing = 0.0;

    height_map = new TerrainHeightMap(this);
    addChild(height_map);

    water_height = -0.3;

    _height_noise = new NoiseGenerator;
}

TerrainDefinition::~TerrainDefinition()
{
    delete _height_noise;
}

void TerrainDefinition::validate()
{
    _height_noise->validate();

    if (height < 1.0)
    {
        height = 1.0;
    }

    /* Get minimal and maximal height */
    _height_noise->getRange(&_min_height, &_max_height);
    _min_height *= height * scaling;
    _max_height *= height * scaling;

    /* TODO Alter with heightmap min/max */
}

void TerrainDefinition::copy(BaseDefinition* _destination) const
{
    TerrainDefinition* destination = (TerrainDefinition*)_destination;

    destination->height = height;
    destination->scaling = scaling;
    destination->shadow_smoothing = shadow_smoothing;

    height_map->copy(destination->height_map);

    destination->water_height = water_height;

    _height_noise->copy(destination->_height_noise);

    destination->validate();
}

void TerrainDefinition::save(PackStream* stream) const
{
    BaseDefinition::save(stream);

    stream->write(&height);
    stream->write(&scaling);
    stream->write(&shadow_smoothing);
    stream->write(&water_height);
    _height_noise->save(stream);
}

void TerrainDefinition::load(PackStream* stream)
{
    BaseDefinition::load(stream);

    stream->read(&height);
    stream->read(&scaling);
    stream->read(&shadow_smoothing);
    stream->read(&water_height);
    _height_noise->load(stream);

    validate();
}

double TerrainDefinition::getGridHeight(int x, int z, int with_painting)
{
    double h;

    if (!with_painting || !height_map->getGridHeight(x, z, &h))
    {
        h = _height_noise->get2DTotal((double)x, (double)z);
    }

    return h;
}

double TerrainDefinition::getInterpolatedHeight(double x, double z, int scaled, int with_painting)
{
    double h;
    x /= scaling;
    z /= scaling;

    if (!with_painting || !height_map->getInterpolatedHeight(x, z, &h))
    {
        h = _height_noise->get2DTotal(x, z);
    }

    if (scaled)
    {
        return h * height * scaling;
    }
    else
    {
        return h;
    }
}

HeightInfo TerrainDefinition::getHeightInfo()
{
    HeightInfo result;

    result.min_height = _min_height;
    result.max_height = _max_height;
    /* TODO This is duplicated in ter_render.c (_realGetWaterHeight) */
    result.base_height = water_height * height * scaling;

    return result;
}

double TerrainDefinition::getWaterHeight()
{
    return water_height * height * scaling;
}

unsigned long TerrainDefinition::getMemoryStats()
{
    return height_map->getMemoryStats();
}

void TerrainDefinition::applyPreset(TerrainPreset preset)
{
    int resolution = 8;
    switch (preset)
    {
    case TERRAIN_PRESET_STANDARD:
        _height_noise->randomizeOffsets();
        _height_noise->clearLevels();
        _height_noise->addLevelSimple(pow(2.0, resolution + 1), -1.0, 1.0);
        _height_noise->addLevelsSimple(resolution - 2, pow(2.0, resolution - 1), -0.7, 0.7, 0.5);
        _height_noise->normalizeAmplitude(-1.0, 1.0, 0);
        _height_noise->setFunctionParams(NOISE_FUNCTION_SIMPLEX, 0.0, 0.0);
        scaling = 1.0;
        height = 30.0;
        shadow_smoothing = 0.03;
        break;
    default:
        ;
    }

    height_map->clearPainting();
    validate();
}
