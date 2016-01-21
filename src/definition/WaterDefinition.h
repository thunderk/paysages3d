#ifndef WATERDEFINITION_H
#define WATERDEFINITION_H

#include "definition_global.h"

#include "DefinitionNode.h"
#include "DefinitionWatcher.h"

namespace paysages {
namespace definition {

class DEFINITIONSHARED_EXPORT WaterDefinition : public DefinitionNode {
  public:
    WaterDefinition(DefinitionNode *parent);
    virtual ~WaterDefinition();

    virtual void save(PackStream *stream) const override;
    virtual void load(PackStream *stream) override;

    virtual void copy(DefinitionNode *destination) const override;
    virtual void validate() override;

    inline IntNode *propModel() const {
        return model;
    }
    inline FloatNode *propReflection() const {
        return reflection;
    }
    inline FloatNode *propXOffset() const {
        return xoffset;
    }
    inline FloatNode *propZOffset() const {
        return zoffset;
    }
    inline ColorNode *propDepthColor() const {
        return depth_color;
    }

    typedef enum { WATER_PRESET_LAKE, WATER_PRESET_SEA } WaterPreset;
    void applyPreset(WaterPreset preset, RandomGenerator &random = RandomGeneratorDefault);

  protected:
    virtual void onChildChanged(int depth, const string &relpath) override;

  public:
    double transparency;
    SurfaceMaterial *material;
    double transparency_depth;
    double lighting_depth;

    double scaling;
    double turbulence;
    double waves_height;
    double detail_height;

    double foam_coverage;
    SurfaceMaterial *foam_material;

    NoiseState *noise_state;

  private:
    IntNode *model;
    FloatNode *reflection;
    FloatNode *xoffset;
    FloatNode *zoffset;
    ColorNode *depth_color;
};
}
}

#endif // WATERDEFINITION_H
