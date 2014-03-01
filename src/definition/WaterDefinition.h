#ifndef WATERDEFINITION_H
#define WATERDEFINITION_H

#include "definition_global.h"

#include "BaseDefinition.h"

namespace paysages {
namespace definition {

class DEFINITIONSHARED_EXPORT WaterDefinition: public BaseDefinition
{
public:
    WaterDefinition(BaseDefinition* parent);
    virtual ~WaterDefinition();

    virtual void save(PackStream* stream) const override;
    virtual void load(PackStream* stream) override;

    virtual void copy(BaseDefinition* destination) const override;
    virtual void validate() override;

public:
    typedef enum
    {
        WATER_PRESET_LAKE,
        WATER_PRESET_SEA
    } WaterPreset;
    void applyPreset(WaterPreset preset);

public:
    double transparency;
    double reflection;
    SurfaceMaterial* material;
    Color* depth_color;
    double transparency_depth;
    double lighting_depth;

    double scaling;
    double turbulence;
    double waves_height;
    double detail_height;

    double foam_coverage;
    SurfaceMaterial* foam_material;

    NoiseGenerator* _waves_noise;
};

}
}

#endif // WATERDEFINITION_H
