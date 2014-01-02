#ifndef TEXTURELAYERCOVERAGEPREVIEWRENDERER_H
#define TEXTURELAYERCOVERAGEPREVIEWRENDERER_H

#include "preview_global.h"

#include "Base2dPreviewRenderer.h"

namespace paysages {
namespace preview {

class PREVIEWSHARED_EXPORT TextureLayerCoveragePreviewRenderer: public Base2dPreviewRenderer
{
public:
    TextureLayerCoveragePreviewRenderer();

    void setTextures(TexturesDefinition* textures);
    void setTerrain(TerrainDefinition* terrain);
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

#endif // TEXTURELAYERCOVERAGEPREVIEWRENDERER_H
