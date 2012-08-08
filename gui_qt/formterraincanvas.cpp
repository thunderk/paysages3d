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
        
        configScaling(1.0, 1.0, 1.0, 1.0);
        //configScrolling(-1000.0, 1000.0, 0.0, -1000.0, 1000.0, 0.0);
    }
    
    ~PreviewTerrainCanvasHeight()
    {
        terrainCanvasDelete(_preview_canvas);
    }
protected:
    QColor getColor(double x, double y)
    {
        Color col, mask;
        double height;

        if (_max - _min < 0.000001)
        {
            return Qt::black;
        }
        else
        {
            height = heightmapGetValue(&_preview_canvas->height_map, x + 0.5, y + 0.5);
            col.r = col.g = col.b = (height - _min) / (_max - _min);
            col.a = 1.0;
            
            mask.r = 0.3;
            mask.g = 0.0;
            mask.b = 0.0;
            mask.a = 1.0 - terrainCanvasGetMaskValue(_preview_canvas, x + 0.5, y + 0.5);
            colorMask(&col, &mask);
            
            return colorToQColor(col);
        }
    }
    void updateData()
    {
        terrainCanvasCopy(_base_canvas, _preview_canvas);
        heightmapGetLimits(&_preview_canvas->height_map, &_min, &_max);
    }
private:
    TerrainCanvas* _base_canvas;
    TerrainCanvas* _preview_canvas;
    double _max, _min;
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

void FormTerrainCanvas::afterLayerAdded(void* layer_definition)
{
    terrainCanvasRevertToTerrain((TerrainCanvas*)layer_definition);
}
