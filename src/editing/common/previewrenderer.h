#ifndef PREVIEWRENDERER_H
#define PREVIEWRENDERER_H

#include "rendering_global.h"

#include "Base2dPreviewRenderer.h"
#include "renderer.h"

class BasePreview;

class PreviewRenderer : public Base2dPreviewRenderer
{
public:
    PreviewRenderer();
    virtual ~PreviewRenderer();

    virtual void bindEvent(BasePreview* preview);
    virtual void updateEvent();
    virtual Color getColor2D(double x, double y, double scaling) const override;
};

#endif // PREVIEWRENDERER_H
