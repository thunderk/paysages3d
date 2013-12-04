#ifndef CLOUDBASICLAYERRENDERER_H
#define CLOUDBASICLAYERRENDERER_H

#include "software_global.h"

#include "BaseCloudLayerRenderer.h"

#include "tools/lighting.h"

namespace paysages {
namespace software {

/*!
 * \brief Basic cloud layer renderer.
 *
 * This renderer simply iters through the cloud layer, collecting cloud segments.
 * It does not account for local density variations.
 */
class SOFTWARESHARED_EXPORT CloudBasicLayerRenderer: public BaseCloudLayerRenderer
{
public:
    CloudBasicLayerRenderer(SoftwareRenderer* parent);

    virtual Color getColor(BaseCloudsModel *model, const Vector3 &eye, const Vector3 &location) override;
    virtual bool alterLight(BaseCloudsModel *model, LightDefinition* light, const Vector3 &eye, const Vector3 &location) override;
};

}
}

#endif // CLOUDBASICLAYERRENDERER_H
