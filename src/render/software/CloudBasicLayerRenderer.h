#ifndef CLOUDBASICLAYERRENDERER_H
#define CLOUDBASICLAYERRENDERER_H

#include "software_global.h"

#include "BaseCloudLayerRenderer.h"

#include "tools/lighting.h"

namespace paysages {
namespace software {

class SOFTWARESHARED_EXPORT CloudBasicLayerRenderer: public BaseCloudLayerRenderer
{
public:
    CloudBasicLayerRenderer(SoftwareRenderer* parent);

    virtual double getDensity(CloudLayerDefinition* layer, const Vector3 &location) override;
    virtual Color getColor(CloudLayerDefinition* layer, const Vector3 &eye, const Vector3 &location) override;
    virtual bool alterLight(CloudLayerDefinition* layer, LightDefinition* light, const Vector3 &eye, const Vector3 &location) override;
};

}
}

#endif // CLOUDBASICLAYERRENDERER_H
