#ifndef PREVIEWTERRAINSHAPE_H
#define PREVIEWTERRAINSHAPE_H

#include "basepreview.h"
#include "rendering/renderer.h"

class PreviewTerrainShape : public BasePreview
{
    Q_OBJECT
public:
    explicit PreviewTerrainShape(QWidget *parent = 0);

protected:
    virtual Color getColor(double x, double y);
    virtual void updateData();

private:
    Renderer* _renderer;
};

#endif // PREVIEWTERRAINSHAPE_H
