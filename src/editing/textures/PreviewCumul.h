#ifndef EDITING_TEXTURES_PREVIEWCUMUL_H
#define EDITING_TEXTURES_PREVIEWCUMUL_H

#include "editing/common/previewrenderer.h"

class BasePreview;
class TexturesDefinition;

class PreviewCumul : public PreviewRenderer
{
public:
    explicit PreviewCumul(QObject *parent = 0):
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
