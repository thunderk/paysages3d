#ifndef EDITING_TEXTURES_PREVIEWLAYERCOVERAGE_H
#define EDITING_TEXTURES_PREVIEWLAYERCOVERAGE_H

#include "editing/common/previewrenderer.h"

class BasePreview;

class PreviewLayerCoverage : public PreviewRenderer
{
public:
    void setTextures(TexturesDefinition* textures);
    void setLayer(int layer);

protected:
    virtual void bindEvent(BasePreview* preview);
    virtual void updateEvent();
    virtual Color getColor2D(double x, double y, double scaling);

private:
    TexturesDefinition* textures = 0;
    int layer;
};

#endif
