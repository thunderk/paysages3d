#ifndef WATERCOVERAGEPREVIEWRENDERER_H
#define WATERCOVERAGEPREVIEWRENDERER_H

#include "preview_global.h"

#include "Base2dPreviewRenderer.h"

namespace paysages {
namespace preview {

class PREVIEWSHARED_EXPORT WaterCoveragePreviewRenderer : public Base2dPreviewRenderer
{
public:
    WaterCoveragePreviewRenderer(WaterDefinition* definition);

    virtual void bindEvent(BasePreview* preview) override;
    virtual void updateEvent() override;
    virtual Color getColor2D(double x, double y, double scaling) override;

private:
    WaterDefinition* definition;
    bool highlight;
};

}
}

#endif // WATERCOVERAGEPREVIEWRENDERER_H
