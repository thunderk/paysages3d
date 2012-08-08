#include "formterraincanvas.h"

/**************** Form ****************/
class PreviewTerrainCanvasHeight:public BasePreview
{
public:
    PreviewTerrainCanvasHeight(QWidget* parent, TerrainCanvas* canvas):BasePreview(parent)
    {
        _base_canvas = canvas;
        _preview_canvas = terrainCanvasCreate();
        
        //addOsd(QString("geolocation"));
        
        configScaling(0.5, 200.0, 3.0, 50.0);
        configScrolling(-1000.0, 1000.0, 0.0, -1000.0, 1000.0, 0.0);
    }
    
    ~PreviewTerrainCanvasHeight()
    {
        terrainCanvasDelete(_preview_canvas);
    }
protected:
    QColor getColor(double x, double y)
    {
        double height;

        height = 0.0; // TODO
        return QColor((int)(255.0 * height), (int)(255.0 * height), (int)(255.0 * height));
    }
    void updateData()
    {
        terrainCanvasCopy(_base_canvas, _preview_canvas);
    }
private:
    TerrainCanvas* _base_canvas;
    TerrainCanvas* _preview_canvas;
};

/**************** Form ****************/
FormTerrainCanvas::FormTerrainCanvas(QWidget *parent, Layers* layers):
    BaseFormLayer(parent, layers)
{
    _definition = terrainCanvasCreate();
    
    // TODO Area
    //addInputDouble(tr("Apply at height"), &_definition->offset_y, -20.0, 20.0, 0.1, 1.0);
    addInputHeightMap(tr("Height map"), &_definition->height_map, _definition);
    //addInputDouble(tr("Canvas height"), &_definition->height_factor, 0.0, 20.0, 0.1, 1.0);
    addInputNoise(tr("Detail noise"), _definition->detail_noise);
    addInputDouble(tr("Detail noise height"), &_definition->detail_height_factor, 0.0, 20.0, 0.1, 1.0);
    addInputDouble(tr("Detail noise scaling"), &_definition->detail_scaling, 0.0, 20.0, 0.1, 1.0);
    addInputEnum(tr("Mask shape"), &_definition->mask.mode, QStringList(tr("Square")) << tr("Circle"));
    addInputDouble(tr("Mask smoothing"), &_definition->mask.smoothing, 0.0, 1.0, 0.01, 0.1);
    
    _previewHeight = new PreviewTerrainCanvasHeight(this, _definition);
    addPreview(_previewHeight, tr("Height preview (normalized)"));
    
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
