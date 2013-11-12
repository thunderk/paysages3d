#ifndef EDITING_TEXTURES_PREVIEWLAYERCOVERAGE_H
#define EDITING_TEXTURES_PREVIEWLAYERCOVERAGE_H

#include "editing_global.h"

#include "Base2dPreviewRenderer.h"

class TexturesDefinition;

class PreviewLayerCoverage : public Base2dPreviewRenderer
{
public:
    explicit PreviewLayerCoverage():
        Base2dPreviewRenderer(),textures(0){}

    void setTextures(TexturesDefinition* textures);
    void setLayer(int layer);

protected:
    virtual void bindEvent(BasePreview* preview);
    virtual void updateEvent();
    virtual Color getColor2D(double x, double y, double scaling);

private:
    TexturesDefinition* textures;
    int layer;
};

#endif
