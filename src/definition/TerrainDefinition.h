#ifndef TERRAINDEFINITION_H
#define TERRAINDEFINITION_H

#include "definition_global.h"

#include "BaseDefinition.h"

namespace paysages {
namespace definition {

typedef struct
{
    double min_height;
    double max_height;
    double base_height;
} HeightInfo;

class DEFINITIONSHARED_EXPORT TerrainDefinition : public BaseDefinition
{
public:
    TerrainDefinition(BaseDefinition* parent);
    virtual ~TerrainDefinition();

    virtual void save(PackStream* stream) const override;
    virtual void load(PackStream* stream) override;

    virtual void copy(BaseDefinition* destination) const override;
    virtual void validate() override;

    double getGridHeight(int x, int z, int with_painting);
    double getInterpolatedHeight(double x, double z, int scaled, int with_painting);
    unsigned long getMemoryStats();
    HeightInfo getHeightInfo();
    double getWaterHeight();

public:
    typedef enum
    {
        TERRAIN_PRESET_STANDARD
    } TerrainPreset;
    void applyPreset(TerrainPreset preset);

public:
    double height;
    double scaling;
    double shadow_smoothing;

    TerrainHeightMap* height_map;

    double water_height;

    double _detail;
    NoiseGenerator* _height_noise;
    double _min_height;
    double _max_height;
};

}
}

#endif // TERRAINDEFINITION_H
