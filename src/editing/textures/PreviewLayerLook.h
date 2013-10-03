#ifndef EDITING_TEXTURES_PREVIEWLAYERLOOK_H
#define EDITING_TEXTURES_PREVIEWLAYERLOOK_H

#include "editing/common/previewrenderer.h"

class BasePreview;

class PreviewLayerLook : public PreviewRenderer
{
public:
    explicit PreviewLayerLook(QObject *parent = 0):
        PreviewRenderer(parent),textures(0){}

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
