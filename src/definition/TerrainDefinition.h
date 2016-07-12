#pragma once

#include "definition_global.h"

#include "DefinitionNode.h"

namespace paysages {
namespace definition {

typedef struct {
    double min_height;
    double max_height;
    double base_height;
} HeightInfo;

class DEFINITIONSHARED_EXPORT TerrainDefinition : public DefinitionNode {
  public:
    TerrainDefinition(DefinitionNode *parent);
    virtual ~TerrainDefinition();

    virtual void save(PackStream *stream) const override;
    virtual void load(PackStream *stream) override;

    virtual void copy(DefinitionNode *destination) const override;
    virtual void validate() override;

    inline FloatNode *propWaterHeight() const {
        return water_height;
    }
    inline NoiseNode *propHeightNoise() const {
        return height_noise;
    }

    double getGridHeight(int x, int z, bool with_painting);
    double getInterpolatedHeight(double x, double z, bool with_painting, bool water_offset = true);
    double getWaterOffset() const;
    unsigned long getMemoryStats();
    HeightInfo getHeightInfo();

  public:
    typedef enum { TERRAIN_PRESET_STANDARD } TerrainPreset;
    void applyPreset(TerrainPreset preset, RandomGenerator &random = RandomGeneratorDefault);

  public:
    double shadow_smoothing;

    TerrainHeightMap *height_map;
    bool has_painting;

    double _min_height;
    double _max_height;

  private:
    FloatNode *water_height;
    NoiseNode *height_noise;
};
}
}
