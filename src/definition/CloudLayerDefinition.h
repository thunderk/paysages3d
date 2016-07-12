#pragma once

#include "definition_global.h"

#include "DefinitionNode.h"

#include "NoiseState.h"

namespace paysages {
namespace definition {

class DEFINITIONSHARED_EXPORT CloudLayerDefinition : public DefinitionNode {
  public:
    CloudLayerDefinition(DefinitionNode *parent, const string &name);
    virtual ~CloudLayerDefinition();

    inline const NoiseState &getNoiseState() const {
        return noise_state;
    }
    inline FloatNode *propXOffset() const {
        return xoffset;
    }
    inline FloatNode *propZOffset() const {
        return zoffset;
    }

    static CloudLayerDefinition *newCopy(const CloudLayerDefinition &other, DefinitionNode *parent);
    CloudLayerDefinition *newCopy(DefinitionNode *parent) const;

    virtual void save(PackStream *pack) const override;
    virtual void load(PackStream *pack) override;

    virtual void copy(DefinitionNode *destination) const override;
    virtual void validate() override;

  public:
    typedef enum {
        STRATUS,
        NIMBOSTRATUS,
        CUMULUS,
        STRATOCUMULUS,
        ALTOCUMULUS,
        ALTOSTRATUS,
        CUMULONIMBUS,
        CIRROCUMULUS,
        CIRROSTRATUS,
        CIRRUS,
        _COUNT
    } CloudsType;

  public:
    CloudsType type;
    NoiseState noise_state;
    double altitude;
    double scaling;
    double coverage;

  private:
    FloatNode *xoffset;
    FloatNode *zoffset;
};
}
}
