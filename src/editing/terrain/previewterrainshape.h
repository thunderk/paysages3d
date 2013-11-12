#ifndef PREVIEWTERRAINSHAPE_H
#define PREVIEWTERRAINSHAPE_H

#include "common/previewrenderer.h"
#include "renderer.h"
#include "terrain/public.h"

class PreviewTerrainShape : public PreviewRenderer
{
public:
    explicit PreviewTerrainShape(TerrainDefinition* terrain);

protected:
    virtual void bindEvent(BasePreview* preview);
    virtual void updateEvent();
    virtual Color getColor2D(double x, double y, double scaling) const override;
    virtual void toggleChangeEvent(QString key, bool value);

private:
    TerrainDefinition* _terrain;
    bool _highlight_enabled;
    double _water_height;
};

#endif // PREVIEWTERRAINSHAPE_H
