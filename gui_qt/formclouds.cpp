#include "formclouds.h"

#include "../lib_paysages/tools/color.h"
#include "../lib_paysages/tools/euclid.h"
#include "../lib_paysages/scenery.h"

#include "tools.h"

/**************** Previews ****************/
class PreviewCloudsCoverage:public BasePreview
{
public:
    PreviewCloudsCoverage(QWidget* parent, CloudsLayerDefinition* layer):BasePreview(parent)
    {
        _renderer = cloudsCreatePreviewCoverageRenderer();
        _3d = true;

        _original_layer = layer;
        _preview_definition = (CloudsDefinition*)CloudsDefinitionClass.create();

        addToggle("3d", tr("Perspective"), true);
        configScaling(100.0, 1000.0, 20.0, 200.0);
    }
    ~PreviewCloudsCoverage()
    {
        CloudsDefinitionClass.destroy(_preview_definition);
        rendererDelete(_renderer);
    }
protected:
    Color getColor(double x, double y)
    {
        return cloudsGetPreviewCoverage(_renderer, x, y, scaling, _3d);
    }
    virtual void toggleChangeEvent(QString key, bool value)
    {
        if (key == "3d")
        {
            _3d = value;
        }
        BasePreview::toggleChangeEvent(key, value);
    }
    void updateData()
    {
        layersDeleteLayer(_preview_definition->layers, 0);
        layersAddLayer(_preview_definition->layers, _original_layer);
        CloudsRendererClass.bind(_renderer, _preview_definition);
    }

private:
    Renderer* _renderer;
    CloudsLayerDefinition* _original_layer;
    CloudsDefinition* _preview_definition;
    bool _3d;
};

class PreviewCloudsColor:public BasePreview
{
public:
    PreviewCloudsColor(QWidget* parent, CloudsLayerDefinition* layer):BasePreview(parent)
    {
        _original_layer = layer;
        _preview_definition = (CloudsDefinition*)CloudsDefinitionClass.create();

        _renderer = cloudsCreatePreviewColorRenderer();

        configScaling(0.5, 2.0, 0.1, 2.0);
    }
protected:
    Color getColor(double x, double y)
    {
        return cloudsGetPreviewColor(_renderer, x, y);
    }
    void updateData()
    {
        layersDeleteLayer(_preview_definition->layers, 0);
        layersAddLayer(_preview_definition->layers, _original_layer);
        CloudsRendererClass.bind(_renderer, _preview_definition);
    }
private:
    Renderer* _renderer;
    CloudsLayerDefinition* _original_layer;
    CloudsDefinition* _preview_definition;
};

/**************** Form ****************/
FormClouds::FormClouds(QWidget *parent):
    BaseFormLayer(parent)
{
    addAutoPreset(tr("Cirrus"));
    addAutoPreset(tr("Cumulus"));
    addAutoPreset(tr("Stratocumulus"));
    addAutoPreset(tr("Stratus"));

    _definition = (CloudsDefinition*)CloudsDefinitionClass.create();
    _layer = (CloudsLayerDefinition*)cloudsGetLayerType().callback_create();

    _previewCoverage = new PreviewCloudsCoverage(parent, _layer);
    _previewColor = new PreviewCloudsColor(parent, _layer);
    addPreview(_previewCoverage, tr("Layer coverage (no lighting)"));
    addPreview(_previewColor, tr("Appearance"));

    addInputEnum(tr("Clouds model"), (int*)&_layer->type, QStringList() << tr("Cirrus") << tr("Cumulus") << tr("Stratocumulus") << tr("Stratus"));
    addInputDouble(tr("Lower altitude"), &_layer->lower_altitude, -10.0, 50.0, 0.5, 5.0);
    addInputDouble(tr("Layer thickness"), &_layer->thickness, 0.0, 20.0, 0.1, 1.0);
    addInputDouble(tr("Max coverage"), &_layer->base_coverage, 0.0, 1.0, 0.01, 0.1);
    addInputDouble(tr("Shape scaling"), &_layer->shape_scaling, 3.0, 30.0, 0.3, 3.0);
    addInputDouble(tr("Edge scaling"), &_layer->edge_scaling, 0.5, 5.0, 0.05, 0.5);
    addInputDouble(tr("Edge length"), &_layer->edge_length, 0.0, 1.0, 0.01, 0.1);
    addInputMaterial(tr("Material"), &_layer->material);
    addInputDouble(tr("Hardness to light"), &_layer->hardness, 0.0, 1.0, 0.01, 0.1);
    addInputDouble(tr("Transparency depth"), &_layer->transparencydepth, 0.0, 10.0, 0.1, 1.0);
    addInputDouble(tr("Light traversal depth"), &_layer->lighttraversal, 0.0, 10.0, 0.1, 1.0);
    addInputDouble(tr("Minimum lighting"), &_layer->minimumlight, 0.0, 1.0, 0.01, 0.1);

    setLayers(_definition->layers);
}

void FormClouds::revertConfig()
{
    sceneryGetClouds(_definition);
    BaseFormLayer::revertConfig();
}

void FormClouds::applyConfig()
{
    BaseFormLayer::applyConfig();
    scenerySetClouds(_definition);
}

void FormClouds::layerReadCurrentFrom(void* layer_definition)
{
    cloudsGetLayerType().callback_copy((CloudsLayerDefinition*)layer_definition, _layer);
}

void FormClouds::layerWriteCurrentTo(void* layer_definition)
{
    cloudsGetLayerType().callback_copy(_layer, (CloudsLayerDefinition*)layer_definition);
}

void FormClouds::autoPresetSelected(int preset)
{
    cloudsLayerAutoPreset(_layer, (CloudsLayerPreset)preset);
    BaseForm::autoPresetSelected(preset);
}

