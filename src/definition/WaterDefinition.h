#ifndef WATERDEFINITION_H
#define WATERDEFINITION_H

#include "definition_global.h"

#include "DefinitionNode.h"
#include "DefinitionWatcher.h"

namespace paysages {
namespace definition {

class DEFINITIONSHARED_EXPORT WaterDefinition: public DefinitionNode, public DefinitionWatcher
{
public:
    WaterDefinition(DefinitionNode* parent);
    virtual ~WaterDefinition();

    virtual void save(PackStream* stream) const override;
    virtual void load(PackStream* stream) override;

    virtual void copy(DefinitionNode* destination) const override;
    virtual void validate() override;

    inline IntNode *propModel() const {return model;}
    inline FloatNode *propReflection() const {return reflection;}

    virtual void nodeChanged(const DefinitionNode *node, const DefinitionDiff *diff);

    typedef enum
    {
        WATER_PRESET_LAKE,
        WATER_PRESET_SEA
    } WaterPreset;
    void applyPreset(WaterPreset preset);

public:
    double transparency;
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

    NoiseState* noise_state;

private:
    IntNode *model;
    FloatNode *reflection;
};

}
}

#endif // WATERDEFINITION_H
