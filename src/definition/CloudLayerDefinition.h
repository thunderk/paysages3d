#ifndef CLOUDLAYERDEFINITION_H
#define CLOUDLAYERDEFINITION_H

#include "definition_global.h"

#include "BaseDefinition.h"

namespace paysages {
namespace definition {

class DEFINITIONSHARED_EXPORT CloudLayerDefinition : public BaseDefinition
{
public:
    CloudLayerDefinition(BaseDefinition* parent);
    virtual ~CloudLayerDefinition();

    static CloudLayerDefinition* newCopy(const CloudLayerDefinition& other, BaseDefinition* parent);
    CloudLayerDefinition* newCopy(BaseDefinition* parent) const;

    virtual void save(PackStream* pack) const override;
    virtual void load(PackStream* pack) override;

    virtual void copy(BaseDefinition* destination) const override;
    virtual void validate() override;

public:
    typedef enum
    {
        STRATUS,
        NIMBOSTRATUS,
        CUMULUS,
        STRATOCUMULUS,
        ALTOCUMULUS,
        ALTOSTRATUS,
        CUMULONIMBUS,
        CIRROCUMULUS,
        CIRROSTRATUS,
        CIRRUS
    } CloudsType;

public:
    CloudsType type;
    double altitude;
    double scaling;
    double coverage;
};

}
}

#endif // CLOUDLAYERDEFINITION_H
