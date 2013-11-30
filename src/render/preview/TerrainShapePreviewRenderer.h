#ifndef TERRAINSHAPEPREVIEWRENDERER_H
#define TERRAINSHAPEPREVIEWRENDERER_H

#include "preview_global.h"

#include "Base2dPreviewRenderer.h"

namespace paysages {
namespace preview {

class TerrainShapePreviewRenderer : public Base2dPreviewRenderer
{
public:
    explicit TerrainShapePreviewRenderer(TerrainDefinition* terrain);

protected:
    virtual void bindEvent(BasePreview* preview);
    virtual void updateEvent();
    virtual Color getColor2D(double x, double y, double scaling) override;
    virtual void toggleChangeEvent(QString key, bool value);

    TerrainDefinition* _terrain;

private:
    bool _highlight_enabled;
    double _water_height;
};

}
}

#endif // TERRAINSHAPEPREVIEWRENDERER_H
