#ifndef CLOUDSDEFINITION_H
#define CLOUDSDEFINITION_H

#include "definition_global.h"

#include "Layers.h"

namespace paysages {
namespace definition {

class DEFINITIONSHARED_EXPORT CloudsDefinition : public Layers {
  public:
    CloudsDefinition(DefinitionNode *parent);

    inline CloudLayerDefinition *getCloudLayer(int position) const {
        return (CloudLayerDefinition *)getLayer(position);
    }

    typedef enum { CLOUDS_PRESET_PARTLY_CLOUDY } CloudsPreset;
    void applyPreset(CloudsPreset preset);
};
}
}

#endif // CLOUDSDEFINITION_H
