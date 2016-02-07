#include "TextureLayerDefinition.h"

#include <algorithm>
#include "Zone.h"
#include "SurfaceMaterial.h"
#include "PackStream.h"
#include "Scenery.h"
#include "TerrainDefinition.h"
#include "Color.h"
#include "FractalNoise.h"
#include "NoiseNode.h"

TextureLayerDefinition::TextureLayerDefinition(DefinitionNode *parent, const string &name)
    : DefinitionNode(parent, name, "texturelayer") {
    terrain_zone = new Zone;
    material = new SurfaceMaterial;

    displacement_noise = new NoiseNode(this, "displacement");
    displacement_noise->setConfig(0.0);

    detail_noise = new NoiseNode(this, "detail");
    detail_noise->setConfig(0.01);
}

TextureLayerDefinition::~TextureLayerDefinition() {
    delete terrain_zone;
    delete material;
}

void TextureLayerDefinition::validate() {
    DefinitionNode::validate();

    material->validate();

    // Update zone height range
    if (auto scenery = getScenery()) {
        TerrainDefinition *terrain = scenery->getTerrain();
        HeightInfo height_info = terrain->getHeightInfo();
        terrain_zone->setRelativeHeight(height_info.min_height, height_info.base_height, height_info.max_height);
    }
}

void TextureLayerDefinition::copy(DefinitionNode *destination) const {
    DefinitionNode::copy(destination);

    if (auto tex_destination = dynamic_cast<TextureLayerDefinition *>(destination)) {
        terrain_zone->copy(tex_destination->terrain_zone);

        *tex_destination->material = *material;
    }
}

void TextureLayerDefinition::save(PackStream *stream) const {
    DefinitionNode::save(stream);

    terrain_zone->save(stream);
    material->save(stream);
}

void TextureLayerDefinition::load(PackStream *stream) {
    DefinitionNode::load(stream);

    terrain_zone->load(stream);
    material->load(stream);
}

void TextureLayerDefinition::applyPreset(TextureLayerPreset preset, RandomGenerator &random) {
    displacement_noise->randomize(random);
    detail_noise->randomize(random);

    terrain_zone->clear();

    switch (preset) {
    case TEXTURES_LAYER_PRESET_MUD:
        displacement_noise->setConfig(0.05, 0.1, 0.3);
        detail_noise->setConfig(0.01, 0.03);
        material->setColor(0.015, 0.014, 0.014, 1.0);
        material->reflection = 0.003;
        material->shininess = 4.0;
        break;
    case TEXTURES_LAYER_PRESET_ROCK:
        terrain_zone->addHeightRangeQuick(1.0, 0.6, 0.7, 0.87, 0.95);
        displacement_noise->setConfig(4.0, 0.15, 0.5, 0.8);
        detail_noise->setConfig(0.02, 0.04);
        material->setColor(0.6, 0.55, 0.57, 1.0);
        material->reflection = 0.006;
        material->shininess = 6.0;
        break;
    case TEXTURES_LAYER_PRESET_GRASS:
        terrain_zone->addHeightRangeQuick(1.0, 0.45, 0.5, 0.7, 0.9);
        terrain_zone->addSlopeRangeQuick(1.0, 0.0, 0.0, 0.05, 0.4);
        displacement_noise->setConfig(2.0, 0.05);
        detail_noise->setConfig(0.01, 0.1);
        material->setColor(0.12, 0.19, 0.035, 1.0);
        material->reflection = 0.001;
        material->shininess = 4.0;
        break;
    case TEXTURES_LAYER_PRESET_SAND:
        terrain_zone->addHeightRangeQuick(1.0, 0.495, 0.505, 0.56, 0.63);
        terrain_zone->addSlopeRangeQuick(1.0, 0.0, 0.0, 0.05, 0.3);
        displacement_noise->setConfig(0.04, 0.1, 0.5, 0.3);
        detail_noise->setConfig(0.004, 0.08);
        material->setColor(1.2, 1.1, 0.9, 1.0);
        material->reflection = 0.008;
        material->shininess = 1.0;
        break;
    case TEXTURES_LAYER_PRESET_SNOW:
        terrain_zone->addHeightRangeQuick(1.0, 0.87, 0.95, 10.0, 100.0);
        terrain_zone->addSlopeRangeQuick(1.0, 0.0, 0.0, 0.1, 1.0);
        displacement_noise->setConfig(0.4, 0.07);
        detail_noise->setConfig(0.01, 0.03);
        material->setColor(5.0, 5.0, 5.0, 1.0);
        material->reflection = 0.02;
        material->shininess = 0.6;
        break;
    }

    validate();
}

bool TextureLayerDefinition::hasDisplacement() const {
    return displacement_noise->getGenerator()->getHeight() > 0.0;
}

double TextureLayerDefinition::getMaximalDisplacement() const {
    double neg, pos;
    displacement_noise->getGenerator()->estimateRange(&neg, &pos);
    return max(fabs(neg), fabs(pos));
}
