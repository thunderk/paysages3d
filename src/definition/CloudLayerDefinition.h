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
        CLOUDS_TYPE_CIRRUS,
        CLOUDS_TYPE_CUMULUS,
        CLOUDS_TYPE_STRATOCUMULUS,
        CLOUDS_TYPE_STRATUS
    } CloudsType;

    typedef enum
    {
        CLOUDS_LAYER_PRESET_CIRRUS,
        CLOUDS_LAYER_PRESET_CUMULUS,
        CLOUDS_LAYER_PRESET_STRATOCUMULUS,
        CLOUDS_LAYER_PRESET_STRATUS
    } CloudsLayerPreset;
    void applyPreset(CloudsLayerPreset preset);

public:
    CloudsType type;
    double lower_altitude;
    double thickness;
    double base_coverage;
    double shape_scaling;
    double edge_scaling;
    double edge_length;
    SurfaceMaterial* material;
    double hardness;
    double transparencydepth;
    double lighttraversal;
    double minimumlight;

    Curve* _coverage_by_altitude;
    NoiseGenerator* _coverage_noise;
    NoiseGenerator* _shape_noise;
    NoiseGenerator* _edge_noise;
};

}
}

#endif // CLOUDLAYERDEFINITION_H
