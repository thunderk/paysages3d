#ifndef TEXTURELAYERASPECTPREVIEWRENDERER_H
#define TEXTURELAYERASPECTPREVIEWRENDERER_H

#include "preview_global.h"

#include "Base2dPreviewRenderer.h"

namespace paysages {
namespace preview {

class PREVIEWSHARED_EXPORT TextureLayerAspectPreviewRenderer: public Base2dPreviewRenderer
{
public:
    TextureLayerAspectPreviewRenderer();

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

}
}

#endif // TEXTURELAYERASPECTPREVIEWRENDERER_H
