#ifndef CLOUDBASICLAYERRENDERER_H
#define CLOUDBASICLAYERRENDERER_H

#include "software_global.h"

#include "BaseCloudLayerRenderer.h"

typedef struct CloudSegment CloudSegment;

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
    virtual bool alterLight(BaseCloudsModel *model, LightComponent* light, const Vector3 &eye, const Vector3 &location) override;

private:
    int findSegments(BaseCloudsModel* model, const Vector3 &start, const Vector3 &direction, int max_segments, double max_inside_length, double max_total_length, double* inside_length, double* total_length, CloudSegment* out_segments);
};

}
}

#endif // CLOUDBASICLAYERRENDERER_H
