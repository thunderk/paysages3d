#ifndef CLOUDSCOVERAGEPREVIEWRENDERER_H
#define CLOUDSCOVERAGEPREVIEWRENDERER_H

#include "preview_global.h"

#include "Base2dPreviewRenderer.h"

namespace paysages {
namespace preview {

class PREVIEWSHARED_EXPORT CloudsCoveragePreviewRenderer : public Base2dPreviewRenderer
{
public:
    CloudsCoveragePreviewRenderer(CloudLayerDefinition* layer);

    virtual void bindEvent(BasePreview* preview) override;
    virtual void updateEvent() override;
    virtual Color getColor2D(double x, double y, double scaling) override;

private:
    bool perspective;
    CloudLayerDefinition* layer;
};

}
}

#endif // CLOUDSCOVERAGEPREVIEWRENDERER_H
