#include "formtextures.h"

#include "../lib_paysages/textures.h"
#include "../lib_paysages/scenery.h"
#include "tools.h"

static TexturesDefinition _definition;
static TextureLayerDefinition _layer;

typedef struct
{
    double height_soft_min;
    double height_hard_min;
    double height_hard_max;
    double height_soft_max;
    double slope_soft_min;
    double slope_hard_min;
    double slope_hard_max;
    double slope_soft_max;
} TextureSupp;
static TextureSupp _supp;

/**************** Previews ****************/
class PreviewTexturesCoverage:public BasePreview
{
public:
    PreviewTexturesCoverage(QWidget* parent):BasePreview(parent)
    {
        _terrain = terrainCreateDefinition();
        
        _renderer = rendererCreate();
        _renderer.render_quality = 3;
        _renderer.getTerrainHeight = _getTerrainHeight;
        _renderer.customData[0] = &_terrain;
        
        _preview_layer = texturesLayerCreateDefinition();
    }
protected:
    QColor getColor(double x, double y)
    {
        Vector3 location;
        double coverage;
        location.x = x;
        location.y = terrainGetHeight(&_terrain, x, y);
        location.z = y;
        coverage = texturesGetLayerCoverage(&_preview_layer, &_renderer, location, this->scaling);
        return QColor::fromRgbF(coverage, coverage, coverage, 1.0);
    }
    void updateData()
    {
        sceneryGetTerrain(&_terrain);
        texturesLayerCopyDefinition(&_layer, &_preview_layer);
    }

private:
    static double _getTerrainHeight(Renderer* renderer, double x, double z)
    {
        return terrainGetHeight((TerrainDefinition*)(renderer->customData[0]), x, z);
    }

    Renderer _renderer;
    TextureLayerDefinition _preview_layer;
    TerrainDefinition _terrain;
};

class PreviewTexturesColor:public BasePreview
{
public:
    PreviewTexturesColor(QWidget* parent):BasePreview(parent)
    {
        LightDefinition light;

        _preview_layer = texturesLayerCreateDefinition();
        
        _lighting = lightingCreateDefinition();
        light.color = COLOR_WHITE;
        light.amplitude = 0.0;
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
        _renderer.applyLightingToSurface = _applyLightingToSurface;
        _renderer.customData[0] = &_lighting;
    }
protected:
    QColor getColor(double x, double y)
    {
        Vector3 location;
        location.x = x * 0.01;
        location.y = 0.0;
        location.z = y * 0.01;
        return colorToQColor(texturesGetLayerColor(&_preview_layer, &_renderer, location, this->scaling * 0.01));
    }
    void updateData()
    {
        texturesLayerCopyDefinition(&_layer, &_preview_layer);
    }
private:
    static Color _applyLightingToSurface(Renderer* renderer, Vector3 location, Vector3 normal, SurfaceMaterial material)
    {
        return lightingApplyToSurface((LightingDefinition*)renderer->customData[0], renderer, location, normal, material);
    }
    
    Renderer _renderer;
    TextureLayerDefinition _preview_layer;
    LightingDefinition _lighting;
};

/**************** Form ****************/
FormTextures::FormTextures(QWidget *parent):
    BaseForm(parent, false, true)
{
    _definition = texturesCreateDefinition();
    _layer = texturesLayerCreateDefinition();

    previewCoverage = new PreviewTexturesCoverage(this);
    previewColor = new PreviewTexturesColor(this);
    addPreview(previewCoverage, tr("Coverage preview"));
    addPreview(previewColor, tr("Lighted sample"));

    addInputNoise(tr("Surface noise"), _layer.bump_noise);
    addInputDouble(tr("Surface noise height"), &_layer.bump_height, 0.0, 0.5, 0.001, 0.05);
    addInputDouble(tr("Surface noise scaling"), &_layer.bump_scaling, 0.1, 3.0, 0.01, 0.1);
    addInputColor(tr("Base color"), &_layer.material.base);
    addInputDouble(tr("Light reflection"), &_layer.material.reflection, 0.0, 1.0, 0.01, 0.1);
    addInputDouble(tr("Light reflection shininess"), &_layer.material.shininess, 0.0, 20.0, 0.1, 1.0);

    addInputDouble(tr("Hard minimal height"), &_supp.height_hard_min, -20.0, 20.0, 0.1, 1.0);
    addInputDouble(tr("Soft minimal height"), &_supp.height_soft_min, -20.0, 20.0, 0.1, 1.0);
    addInputDouble(tr("Soft maximal height"), &_supp.height_soft_max, -20.0, 20.0, 0.1, 1.0);
    addInputDouble(tr("Hard maximal height"), &_supp.height_hard_max, -20.0, 20.0, 0.1, 1.0);
    addInputDouble(tr("Hard minimal slope"), &_supp.slope_hard_min, 0.0, 5.0, 0.05, 0.5);
    addInputDouble(tr("Soft minimal slope"), &_supp.slope_soft_min, 0.0, 5.0, 0.05, 0.5);
    addInputDouble(tr("Soft maximal slope"), &_supp.slope_soft_max, 0.0, 5.0, 0.05, 0.5);
    addInputDouble(tr("Hard maximal slope"), &_supp.slope_hard_max, 0.0, 5.0, 0.05, 0.5);

    revertConfig();
}

void FormTextures::revertConfig()
{
    sceneryGetTextures(&_definition);
    setLayerCount(texturesGetLayerCount(&_definition));
    BaseForm::revertConfig();
}

void FormTextures::applyConfig()
{
    scenerySetTextures(&_definition);
    BaseForm::applyConfig();
}

void FormTextures::configChangeEvent()
{
    ZoneRangeCondition range;
    
    texturesLayerCopyDefinition(&_layer, texturesGetLayer(&_definition, currentLayer()));
    
    if (zoneGetHeightRangeCount(_layer.zone) > 0)
    {
        range.value = 1.0;
        range.softmin = _supp.height_soft_min;
        range.hardmin = _supp.height_hard_min;
        range.hardmax = _supp.height_hard_max;
        range.softmax = _supp.height_soft_max;
        zoneSetHeightRange(_layer.zone, 0, &range);
    }
    if (zoneGetSlopeRangeCount(_layer.zone) > 0)
    {
        range.value = 1.0;
        range.softmin = _supp.slope_soft_min;
        range.hardmin = _supp.slope_hard_min;
        range.hardmax = _supp.slope_hard_max;
        range.softmax = _supp.slope_soft_max;
        zoneSetSlopeRange(_layer.zone, 0, &range);
    }
    
    texturesValidateDefinition(&_definition);
    BaseForm::configChangeEvent();
}

void FormTextures::layerAddedEvent()
{
    if (texturesAddLayer(&_definition) >= 0)
    {
        BaseForm::layerAddedEvent();
    }
}

void FormTextures::layerDeletedEvent(int layer)
{
    texturesDeleteLayer(&_definition, layer);
    
    BaseForm::layerDeletedEvent(layer);
}

void FormTextures::layerSelectedEvent(int layer)
{
    ZoneRangeCondition range;
    
    texturesLayerCopyDefinition(texturesGetLayer(&_definition, layer), &_layer);
    
    if (zoneGetHeightRangeCount(_layer.zone) == 0)
    {
        zoneAddHeightRange(_layer.zone);
    }
    zoneGetHeightRange(_layer.zone, 0, &range);
    _supp.height_soft_min = range.softmin;
    _supp.height_hard_min = range.hardmin;
    _supp.height_hard_max = range.hardmax;
    _supp.height_soft_max = range.softmax;
    
    if (zoneGetSlopeRangeCount(_layer.zone) == 0)
    {
        zoneAddSlopeRange(_layer.zone);
    }
    zoneGetSlopeRange(_layer.zone, 0, &range);
    _supp.slope_soft_min = range.softmin;
    _supp.slope_hard_min = range.hardmin;
    _supp.slope_hard_max = range.hardmax;
    _supp.slope_soft_max = range.softmax;
    
    BaseForm::layerSelectedEvent(layer);
}
