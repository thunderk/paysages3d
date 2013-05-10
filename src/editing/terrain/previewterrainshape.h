#ifndef PREVIEWTERRAINSHAPE_H
#define PREVIEWTERRAINSHAPE_H

#include "common/previewrenderer.h"
#include "rendering/renderer.h"
#include "rendering/terrain/public.h"

class PreviewTerrainShape : public PreviewRenderer
{
    Q_OBJECT
public:
    explicit PreviewTerrainShape(TerrainDefinition* terrain);

protected:
    virtual void bindEvent(BasePreview* preview);
    virtual void updateEvent();
    virtual Color getColor2D(double x, double y, double scaling);

private:
    TerrainDefinition* _terrain;
};

#endif // PREVIEWTERRAINSHAPE_H
