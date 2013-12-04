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

    virtual bool optimizeSearchLimits(BaseCloudsModel *model, Vector3 *start, Vector3 *end);

    virtual Color getColor(BaseCloudsModel *model, const Vector3 &eye, const Vector3 &location);
    virtual bool alterLight(BaseCloudsModel *model, LightDefinition* light, const Vector3 &eye, const Vector3 &location);

protected:
    SoftwareRenderer* parent;
};

}
}

#endif // BASECLOUDLAYERRENDERER_H
