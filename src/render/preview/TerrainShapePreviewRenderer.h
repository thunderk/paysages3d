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
    virtual Vector3 getCameraLocation(const Vector3 &target) override;

    virtual void bindEvent(BasePreview* preview) override;
    virtual void updateEvent() override;
    virtual Color getColor2D(double x, double y, double scaling) override;

    virtual Color getWaterColor(double x, double y, double scaling);

    TerrainDefinition* _terrain;

private:
    double _water_height;
};

}
}

#endif // TERRAINSHAPEPREVIEWRENDERER_H
