#ifndef _PAYSAGES_QT_FORMTERRAINCANVAS_H_
#define _PAYSAGES_QT_FORMTERRAINCANVAS_H_

#include <QWidget>
#include "basepreview.h"
#include "baseformlayer.h"
#include "../lib_paysages/terraincanvas.h"

class FormTerrainCanvas : public BaseFormLayer
{
    Q_OBJECT

public:
    FormTerrainCanvas(QWidget* parent, Layers* layers);
    ~FormTerrainCanvas();
    
protected:
    virtual void layerGetCopy(void* layer_definition);
    virtual void layerApply(void* layer_definition);
    virtual void afterLayerAdded(void* layer_definition);

private:
    TerrainCanvas* _definition;
    BasePreview* _previewHeight;
    BasePreview* _previewColor;
};

#endif
