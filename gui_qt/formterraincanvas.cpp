#include "formterraincanvas.h"

/**************** Form ****************/
FormTerrainCanvas::FormTerrainCanvas(QWidget *parent, Layers* layers):
    BaseFormLayer(parent, layers)
{
    _definition = terrainCanvasCreate();
    
    // TODO Area
    addInputDouble(tr("Apply at height"), &_definition->offset_y, -20.0, 20.0, 0.1, 1.0);
    addInputHeightMap(tr("Height map"), &_definition->height_map);
    addInputDouble(tr("Canvas height"), &_definition->height_factor, 0.0, 20.0, 0.1, 1.0);
    addInputNoise(tr("Detail noise"), _definition->detail_noise);
    addInputDouble(tr("Detail noise height"), &_definition->detail_height_factor, 0.0, 20.0, 0.1, 1.0);
    addInputDouble(tr("Detail noise scaling"), &_definition->detail_scaling, 0.0, 20.0, 0.1, 1.0);
    addInputEnum(tr("Mask shape"), &_definition->mask.mode, QStringList(tr("Square")) << tr("Circle"));
    addInputDouble(tr("Mask smoothing"), &_definition->mask.smoothing, 0.0, 1.0, 0.01, 0.1);
    
    revertConfig();
}

FormTerrainCanvas::~FormTerrainCanvas()
{
    terrainCanvasDelete(_definition);
}

void FormTerrainCanvas::layerGetCopy(void* layer_definition)
{
    terrainCanvasCopy((TerrainCanvas*)layer_definition, _definition);
}

void FormTerrainCanvas::layerApply(void* layer_definition)
{
    terrainCanvasCopy(_definition, (TerrainCanvas*)layer_definition);
}
