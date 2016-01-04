#ifndef TEXTURELAYERDEFINITION_H
#define TEXTURELAYERDEFINITION_H

#include "definition_global.h"

#include "DefinitionNode.h"

namespace paysages {
namespace definition {

class DEFINITIONSHARED_EXPORT TextureLayerDefinition : public DefinitionNode {
  public:
    typedef enum { TEXTURES_MERGE_FADE, TEXTURES_MERGE_DISSOLVE, TEXTURES_MERGE_DISPLACEMENT_VALUE } TextureMergeMode;
    typedef enum {
        TEXTURES_LAYER_PRESET_MUD,
        TEXTURES_LAYER_PRESET_ROCK,
        TEXTURES_LAYER_PRESET_GRASS,
        TEXTURES_LAYER_PRESET_SAND,
        TEXTURES_LAYER_PRESET_SNOW
    } TextureLayerPreset;

  public:
    TextureLayerDefinition(DefinitionNode *parent, const string &name);
    virtual ~TextureLayerDefinition();

    virtual void save(PackStream *stream) const override;
    virtual void load(PackStream *stream) override;

    virtual void copy(DefinitionNode *destination) const override;
    virtual void validate() override;

    void applyPreset(TextureLayerPreset preset, RandomGenerator &random = RandomGeneratorDefault);

    inline NoiseNode *propDetailNoise() const {
        return detail_noise;
    }

  public:
    Zone *terrain_zone;
    double displacement_scaling;
    double displacement_height;
    SurfaceMaterial *material;

    NoiseGenerator *_displacement_noise;

  private:
    NoiseNode *detail_noise;
};
}
}

#endif // TEXTURELAYERDEFINITION_H
