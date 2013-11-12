#ifndef EDITING_TEXTURES_PREVIEWLAYERCOVERAGE_H
#define EDITING_TEXTURES_PREVIEWLAYERCOVERAGE_H

#include "common/previewrenderer.h"

class BasePreview;
class TexturesDefinition;

class PreviewLayerCoverage : public PreviewRenderer
{
public:
    explicit PreviewLayerCoverage():
        PreviewRenderer(),textures(0){}

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
