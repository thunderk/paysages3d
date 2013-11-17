#include "TextureLayerDefinition.h"

#include "Zone.h"
#include "NoiseGenerator.h"
#include "SurfaceMaterial.h"
#include "PackStream.h"
#include "Scenery.h"
#include "TerrainDefinition.h"

TextureLayerDefinition::TextureLayerDefinition(BaseDefinition* parent):
    BaseDefinition(parent)
{
    terrain_zone = new Zone;
    _displacement_noise = new NoiseGenerator;
    _detail_noise = new NoiseGenerator;
    material = new SurfaceMaterial;
}

TextureLayerDefinition::~TextureLayerDefinition()
{
    delete terrain_zone;
    delete _displacement_noise;
    delete _detail_noise;
    delete material;
}

void TextureLayerDefinition::validate()
{
    BaseDefinition::validate();

    if (displacement_scaling < 0.000001)
    {
        displacement_scaling = 0.000001;
    }

    _displacement_noise->clearLevels();
    _displacement_noise->addLevelsSimple(9, 1.0, -1.0, 1.0, 0.0);
    _displacement_noise->normalizeAmplitude(-1.0, 1.0, 0);
    _displacement_noise->validate();

    _detail_noise->clearLevels();
    _detail_noise->addLevelsSimple(7, 0.01, -1.0, 1.0, 0.0);
    _detail_noise->normalizeAmplitude(-0.008, 0.008, 0);
    _detail_noise->validate();

    materialValidate(material);

    /* Update zone height range */
    Scenery* scenery = getScenery();
    if (scenery)
    {
        TerrainDefinition* terrain = scenery->getTerrain();
        HeightInfo height_info = terrain->getHeightInfo();
        terrain_zone->setRelativeHeight(height_info.min_height, height_info.base_height, height_info.max_height);
    }
}

void TextureLayerDefinition::copy(BaseDefinition *_destination) const
{
    BaseDefinition::copy(_destination);

    TextureLayerDefinition* destination = (TextureLayerDefinition*)_destination;

    terrain_zone->copy(destination->terrain_zone);

    destination->displacement_scaling = displacement_scaling;
    destination->displacement_height = displacement_height;
    destination->displacement_offset = displacement_offset;
    *destination->material = *material;

    _displacement_noise->copy(destination->_displacement_noise);
    _detail_noise->copy(destination->_detail_noise);
}

void TextureLayerDefinition::save(PackStream* stream) const
{
    BaseDefinition::save(stream);

    terrain_zone->save(stream);
    stream->write(&displacement_scaling);
    stream->write(&displacement_height);
    stream->write(&displacement_offset);
    materialSave(stream, material);

    _displacement_noise->save(stream);
    _detail_noise->save(stream);
}

void TextureLayerDefinition::load(PackStream* stream)
{
    BaseDefinition::load(stream);

    terrain_zone->load(stream);
    stream->read(&displacement_scaling);
    stream->read(&displacement_height);
    stream->read(&displacement_offset);
    materialLoad(stream, material);

    _displacement_noise->load(stream);
    _detail_noise->load(stream);
}

void TextureLayerDefinition::applyPreset(TextureLayerPreset preset)
{
    _displacement_noise->randomizeOffsets();
    _detail_noise->randomizeOffsets();

    terrain_zone->clear();

    switch (preset)
    {
        case TEXTURES_LAYER_PRESET_MUD:
            displacement_height = 0.05;
            displacement_scaling = 3.0;
            displacement_offset = 0.0;
            material->base = colorToHSL(colorFromValues(0.4, 0.38, 0.35, 1.0));
            material->reflection = 0.003;
            material->shininess = 4.0;
            break;
        case TEXTURES_LAYER_PRESET_ROCK:
            terrain_zone->addHeightRangeQuick(1.0, 0.5, 0.55, 1.0, 1.0);
            displacement_height = 0.3;
            displacement_scaling = 2.0;
            displacement_offset = 0.0;
            material->base = colorToHSL(colorFromValues(0.6, 0.55, 0.57, 1.0));
            material->reflection = 0.006;
            material->shininess = 6.0;
            break;
        case TEXTURES_LAYER_PRESET_GRASS:
            terrain_zone->addHeightRangeQuick(1.0, 0.45, 0.5, 0.8, 1.0);
            terrain_zone->addSlopeRangeQuick(1.0, 0.0, 0.0, 0.05, 0.4);
            displacement_height = 0.0;
            displacement_scaling = 1.0;
            displacement_offset = 0.0;
            material->base = colorToHSL(colorFromValues(0.12, 0.19, 0.035, 1.0));
            material->reflection = 0.001;
            material->shininess = 4.0;
            break;
        case TEXTURES_LAYER_PRESET_SAND:
            terrain_zone->addHeightRangeQuick(1.0, 0.498, 0.502, 0.505, 0.51);
            terrain_zone->addSlopeRangeQuick(1.0, 0.0, 0.0, 0.7, 0.1);
            displacement_height = 0.02;
            displacement_scaling = 5.0;
            displacement_offset = 0.0;
            material->base = colorToHSL(colorFromValues(0.93, 0.9, 0.8, 1.0));
            material->reflection = 0.008;
            material->shininess = 1.0;
            break;
        case TEXTURES_LAYER_PRESET_SNOW:
            terrain_zone->addHeightRangeQuick(1.0, 0.77, 0.85, 1.0, 1.0);
            terrain_zone->addSlopeRangeQuick(1.0, 0.0, 0.0, 0.2, 1.0);
            displacement_height = 0.1;
            displacement_scaling = 1.0;
            displacement_offset = 0.0;
            material->base = colorToHSL(colorFromValues(1.0, 1.0, 1.0, 1.0));
            material->reflection = 0.25;
            material->shininess = 0.6;
            break;
        default:
            break;
    }

    validate();
}