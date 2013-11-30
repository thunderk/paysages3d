#ifndef WATERASPECTPREVIEWRENDERER_H
#define WATERASPECTPREVIEWRENDERER_H

#include "preview_global.h"

#include "Base2dPreviewRenderer.h"

namespace paysages {
namespace preview {

class PREVIEWSHARED_EXPORT WaterAspectPreviewRenderer : public Base2dPreviewRenderer
{
public:
    WaterAspectPreviewRenderer(WaterDefinition* definition);

    virtual void bindEvent(BasePreview* preview) override;
    virtual void updateEvent() override;
    virtual void cameraEvent(double x, double y, double scaling) override;
    virtual Color getColor2D(double x, double y, double scaling) override;

    virtual void toggleChangeEvent(const std::string &key, bool value) override;
    virtual void choiceChangeEvent(const std::string &key, int position) override;

private:
    WaterDefinition* definition;
    bool lighting;
    int background;
};

}
}

#endif // WATERASPECTPREVIEWRENDERER_H
