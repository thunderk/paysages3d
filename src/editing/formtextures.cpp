#include "formtextures.h"

#include "Scenery.h"
#include "BasePreview.h"
#include "renderer.h"
#include "tools.h"
#include "camera.h"

/**************** Previews ****************/
class PreviewTexturesCoverage : public BasePreview
{
public:

    PreviewTexturesCoverage(QWidget* parent, TexturesLayerDefinition* layer) : BasePreview(parent)
    {
        _renderer = rendererCreate();
        _renderer->render_quality = 3;

        _original_layer = layer;
        //_preview_definition = (TexturesDefinition*)TexturesDefinitionClass.create();

        addOsd(QString("geolocation"));

        configScaling(20.0, 500.0, 20.0, 50.0);
        configScrolling(-1000.0, 1000.0, 0.0, -1000.0, 1000.0, 0.0);
    }

    ~PreviewTexturesCoverage()
    {
        //TexturesDefinitionClass.destroy(_preview_layer);
    }
protected:

    Color getColor(double x, double y)
    {
        Vector3 location;
        Color result;
        location.x = x;
        location.y = _renderer->terrain->getHeight(_renderer, x, y, 1);
        location.z = y;
        //result.r = result.g = result.b = texturesGetLayerCoverage(_preview_layer, _renderer, location, this->scaling);
        return result;
    }

    void updateData()
    {
        TerrainRendererClass.bind(_renderer, Scenery::getCurrent()->getTerrain());

        //TexturesDefinitionClass.copy(_original_layer, _preview_layer);
    }

private:
    Renderer* _renderer;
    TexturesLayerDefinition* _original_layer;
    TexturesDefinition* _preview_definition;
};

class PreviewTexturesColor : public BasePreview
{
public:

    PreviewTexturesColor(QWidget* parent, TexturesLayerDefinition* layer) : BasePreview(parent)
    {
        _original_layer = layer;
        //_preview_layer = (TexturesLayerDefinition*)TexturesDefinitionClass.create();

        _renderer = rendererCreate();
        _renderer->render_quality = 3;

        Vector3 camera_location = {0.0, 20.0, 0.0};
        cameraSetLocation(_renderer->render_camera, camera_location);

        configScaling(0.01, 1.0, 0.01, 0.1);
        configScrolling(-1000.0, 1000.0, 0.0, -1000.0, 1000.0, 0.0);
    }

    ~PreviewTexturesColor()
    {
        //TexturesDefinitionClass.destroy(_preview_layer);
    }
protected:

    Color getColor(double x, double y)
    {
        Vector3 location;
        location.x = x;
        location.y = 0.0;
        location.z = y;
        //return texturesGetLayerColor(_preview_layer, _renderer, location, this->scaling);
        return COLOR_BLACK;
    }

    void updateData()
    {
        //TexturesDefinitionClass.copy(_original_layer, _preview_layer);
    }
private:
    Renderer* _renderer;
    TexturesLayerDefinition* _original_layer;
    TexturesLayerDefinition* _preview_layer;
};

/**************** Form ****************/
FormTextures::FormTextures(QWidget *parent) :
BaseFormLayer(parent)
{
    addAutoPreset(tr("Rock"));
    addAutoPreset(tr("Grass"));
    addAutoPreset(tr("Sand"));
    addAutoPreset(tr("Snow"));

    _definition = (TexturesDefinition*) TexturesDefinitionClass.create();
    _layer = (TexturesLayerDefinition*) texturesGetLayerType().callback_create();

    _previewCoverage = new PreviewTexturesCoverage(this, _layer);
    _previewColor = new PreviewTexturesColor(this, _layer);
    addPreview(_previewCoverage, tr("Coverage preview"));
    addPreview(_previewColor, tr("Lighted sample"));

    addInputDouble(tr("Displacement height"), &_layer->displacement_height, 0.0, 0.1, 0.001, 0.01);
    addInputDouble(tr("Displacement scaling"), &_layer->displacement_scaling, 0.003, 0.3, 0.003, 0.03);
    addInputMaterial(tr("Material"), &_layer->material);
    /*addInputCurve(tr("Coverage by altitude"), _layer->terrain_zone->value_by_height, -20.0, 20.0, 0.0, 1.0, tr("Terrain altitude"), tr("Texture coverage"));
    addInputCurve(tr("Coverage by slope"), _layer->terrain_zone->value_by_slope, 0.0, 5.0, 0.0, 1.0, tr("Terrain slope"), tr("Texture coverage"));*/

    /*addInputDouble(tr("Amplitude for slope coverage"), &_layer->slope_range, 0.001, 0.1, 0.001, 0.01);
    addInputDouble(tr("Layer thickness"), &_layer->thickness, 0.0, 0.1, 0.001, 0.01);
    addInputDouble(tr("Transparency thickness"), &_layer->thickness_transparency, 0.0, 0.1, 0.001, 0.01);*/

    setLayers(_definition->layers);
}

FormTextures::~FormTextures()
{
    TexturesDefinitionClass.destroy(_definition);
    texturesGetLayerType().callback_delete(_layer);
}

void FormTextures::revertConfig()
{
    Scenery::getCurrent()->getTextures(_definition);
    BaseFormLayer::revertConfig();
}

void FormTextures::applyConfig()
{
    BaseFormLayer::applyConfig();
    Scenery::getCurrent()->setTextures(_definition);
}

void FormTextures::layerReadCurrentFrom(void* layer_definition)
{
    texturesGetLayerType().callback_copy((TexturesLayerDefinition*) layer_definition, _layer);
}

void FormTextures::layerWriteCurrentTo(void* layer_definition)
{
    texturesGetLayerType().callback_copy(_layer, (TexturesLayerDefinition*) layer_definition);
}

void FormTextures::autoPresetSelected(int preset)
{
    texturesLayerAutoPreset(_layer, (TexturesLayerPreset) preset);
    BaseForm::autoPresetSelected(preset);
}
