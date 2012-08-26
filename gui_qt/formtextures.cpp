#include "formtextures.h"

#include "../lib_paysages/textures.h"
#include "../lib_paysages/scenery.h"
#include "tools.h"

typedef struct
{
    Curve* height_curve;
    Curve* slope_curve;
} TextureSupp;
static TextureSupp _supp;

/**************** Previews ****************/
class PreviewTexturesCoverage:public BasePreview
{
public:
    PreviewTexturesCoverage(QWidget* parent, TextureLayerDefinition* layer):BasePreview(parent)
    {
        _terrain = terrainCreateDefinition();
        
        _renderer = rendererCreate();
        _renderer.render_quality = 3;
        _renderer.getTerrainHeight = _getTerrainHeight;
        _renderer.customData[0] = &_terrain;
        
        _original_layer = layer;
        _preview_layer = texturesLayerCreateDefinition();
        
        addOsd(QString("geolocation"));
        
        configScaling(0.5, 200.0, 1.0, 50.0);
        configScrolling(-1000.0, 1000.0, 0.0, -1000.0, 1000.0, 0.0);
    }
    ~PreviewTexturesCoverage()
    {
        texturesLayerDeleteDefinition(_preview_layer);
    }
protected:
    QColor getColor(double x, double y)
    {
        Vector3 location;
        double coverage;
        location.x = x;
        location.y = terrainGetHeight(&_terrain, x, y);
        location.z = y;
        coverage = texturesGetLayerCoverage(_preview_layer, &_renderer, location, this->scaling);
        return QColor::fromRgbF(coverage, coverage, coverage, 1.0);
    }
    void updateData()
    {
        sceneryGetTerrain(&_terrain);
        texturesLayerCopyDefinition(_original_layer, _preview_layer);
    }

private:
    static double _getTerrainHeight(Renderer* renderer, double x, double z)
    {
        return terrainGetHeight((TerrainDefinition*)(renderer->customData[0]), x, z);
    }

    Renderer _renderer;
    TextureLayerDefinition* _original_layer;
    TextureLayerDefinition* _preview_layer;
    TerrainDefinition _terrain;
};

class PreviewTexturesColor:public BasePreview
{
public:
    PreviewTexturesColor(QWidget* parent, TextureLayerDefinition* layer):BasePreview(parent)
    {
        LightDefinition light;

        _original_layer = layer;
        _preview_layer = texturesLayerCreateDefinition();
        
        _lighting = lightingCreateDefinition();
        light.color = COLOR_WHITE;
        light.direction.x = 0.0;
        light.direction.y = -0.4794;
        light.direction.z = 0.8776;
        light.filtered = 0;
        light.masked = 0;
        light.reflection = 1.0;
        lightingAddLight(&_lighting, light);
        lightingValidateDefinition(&_lighting);
        
        _renderer = rendererCreate();
        _renderer.render_quality = 3;
        _renderer.getLightStatus = _getLightStatus;
        _renderer.customData[0] = &_lighting;
        _renderer.camera_location.x = 0.0;
        _renderer.camera_location.y = 20.0;
        _renderer.camera_location.z = 0.0;
        
        _zone = zoneCreate();
        
        configScaling(0.01, 1.0, 0.01, 0.1);
        configScrolling(-1000.0, 1000.0, 0.0, -1000.0, 1000.0, 0.0);
    }
    ~PreviewTexturesColor()
    {
        texturesLayerDeleteDefinition(_preview_layer);
    }
protected:
    QColor getColor(double x, double y)
    {
        Vector3 location;
        location.x = x;
        location.y = 0.0;
        location.z = y;
        return colorToQColor(texturesGetLayerColor(_preview_layer, &_renderer, location, this->scaling));
    }
    void updateData()
    {
        texturesLayerCopyDefinition(_original_layer, _preview_layer);
        zoneCopy(_zone, _preview_layer->zone);
    }
private:
    Zone* _zone;
    Renderer _renderer;
    TextureLayerDefinition* _original_layer;
    TextureLayerDefinition* _preview_layer;
    LightingDefinition _lighting;
    
    static void _getLightStatus(Renderer* renderer, LightStatus* status, Vector3 location)
    {
        lightingGetStatus((LightingDefinition*)renderer->customData[0], renderer, location, status);
    }
};

/**************** Form ****************/
FormTextures::FormTextures(QWidget *parent):
    BaseFormLayer(parent)
{
    _definition = texturesCreateDefinition();
    _layer = texturesLayerCreateDefinition();
    _supp.height_curve = curveCreate();
    _supp.slope_curve = curveCreate();

    _previewCoverage = new PreviewTexturesCoverage(this, _layer);
    _previewColor = new PreviewTexturesColor(this, _layer);
    addPreview(_previewCoverage, tr("Coverage preview"));
    addPreview(_previewColor, tr("Lighted sample"));

    addInputNoise(tr("Surface noise"), _layer->bump_noise);
    addInputDouble(tr("Surface noise height"), &_layer->bump_height, 0.0, 0.1, 0.001, 0.01);
    addInputDouble(tr("Surface noise scaling"), &_layer->bump_scaling, 0.003, 0.3, 0.003, 0.03);
    addInputMaterial(tr("Material"), &_layer->material);
    addInputCurve(tr("Coverage by altitude"), _supp.height_curve, -20.0, 20.0, 0.0, 1.0, tr("Terrain altitude"), tr("Texture coverage"));
    addInputCurve(tr("Coverage by slope"), _supp.slope_curve, 0.0, 5.0, 0.0, 1.0, tr("Terrain slope"), tr("Texture coverage"));
    addInputDouble(tr("Amplitude for slope coverage"), &_layer->slope_range, 0.001, 0.1, 0.001, 0.01);
    addInputDouble(tr("Layer thickness"), &_layer->thickness, 0.0, 0.1, 0.001, 0.01);
    addInputDouble(tr("Transparency thickness"), &_layer->thickness_transparency, 0.0, 0.1, 0.001, 0.01);

    setLayers(_definition.layers);
}

FormTextures::~FormTextures()
{
    texturesDeleteDefinition(&_definition);
    texturesLayerDeleteDefinition(_layer);
    curveDelete(_supp.height_curve);
    curveDelete(_supp.slope_curve);
}

void FormTextures::revertConfig()
{
    sceneryGetTextures(&_definition);
    BaseFormLayer::revertConfig();
}

void FormTextures::applyConfig()
{
    BaseFormLayer::applyConfig();
    scenerySetTextures(&_definition);
}

void FormTextures::layerGetCopy(void* layer_definition)
{
    texturesLayerCopyDefinition((TextureLayerDefinition*)layer_definition, _layer);
}

void FormTextures::layerApply(void* layer_definition)
{
    texturesLayerCopyDefinition(_layer, (TextureLayerDefinition*)layer_definition);
}
