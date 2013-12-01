#ifndef BASECLOUDLAYERRENDERER_H
#define BASECLOUDLAYERRENDERER_H

#include "software_global.h"

#include "tools/lighting.h"

namespace paysages {
namespace software {

class BaseCloudLayerRenderer
{
public:
    BaseCloudLayerRenderer(SoftwareRenderer* parent);
    virtual ~BaseCloudLayerRenderer();

    virtual bool optimizeSearchLimits(CloudLayerDefinition *layer, Vector3 *start, Vector3 *end);

    virtual double getDensity(CloudLayerDefinition* layer, const Vector3 &location);
    virtual Color getColor(CloudLayerDefinition* layer, const Vector3 &eye, const Vector3 &location);
    virtual bool alterLight(CloudLayerDefinition* layer, LightDefinition* light, const Vector3 &eye, const Vector3 &location);

protected:
    SoftwareRenderer* parent;
};

}
}

#endif // BASECLOUDLAYERRENDERER_H
