#ifndef WATERDEFINITION_H
#define WATERDEFINITION_H

#include "definition_global.h"

#include "BaseDefinition.h"

namespace paysages {
namespace definition {

class WaterDefinition: public BaseDefinition
{
public:
    WaterDefinition(BaseDefinition* parent);
    virtual ~WaterDefinition();

    virtual void save(PackStream* stream) const override;
    virtual void load(PackStream* stream) override;

    virtual void copy(BaseDefinition* destination) const override;
    virtual void validate() override;

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
