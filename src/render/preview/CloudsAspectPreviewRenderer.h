#ifndef CLOUDSASPECTPREVIEWRENDERER_H
#define CLOUDSASPECTPREVIEWRENDERER_H

#include "preview_global.h"

#include "Base2dPreviewRenderer.h"

namespace paysages {
namespace preview {

class PREVIEWSHARED_EXPORT CloudsAspectPreviewRenderer : public Base2dPreviewRenderer
{
public:
    CloudsAspectPreviewRenderer(CloudLayerDefinition* layer);

    virtual void bindEvent(BasePreview* preview) override;
    virtual void updateEvent() override;
    virtual Color getColor2D(double x, double y, double scaling) override;

private:
    CloudLayerDefinition* layer;
};

}
}

#endif // CLOUDSASPECTPREVIEWRENDERER_H
