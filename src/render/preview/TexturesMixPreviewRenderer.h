#ifndef TEXTURESMIXPREVIEWRENDERER_H
#define TEXTURESMIXPREVIEWRENDERER_H

#include "preview_global.h"

#include "Base2dPreviewRenderer.h"

namespace paysages {
namespace preview {

class PREVIEWSHARED_EXPORT TexturesMixPreviewRenderer:public Base2dPreviewRenderer
{
public:
    TexturesMixPreviewRenderer();

    void setTextures(TexturesDefinition* textures);
    void setTerrain(TerrainDefinition* terrain);
    void setLayer(int layer);

protected:
    virtual void bindEvent(BasePreview* preview);
    virtual void updateEvent();
    virtual Color getColor2D(double x, double y, double scaling);

private:
    int layer;
    TexturesDefinition* textures;
};

}
}

#endif // TEXTURESMIXPREVIEWRENDERER_H
