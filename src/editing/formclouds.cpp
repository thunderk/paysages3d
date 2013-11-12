#include "formclouds.h"

#include "clouds/clo_preview.h"
#include "tools/color.h"
#include "tools/euclid.h"
#include "Scenery.h"

#include "tools.h"

/**************** Previews ****************/
class PreviewCloudsCoverage:public BasePreview
{
public:
    PreviewCloudsCoverage(QWidget* parent, CloudsLayerDefinition* layer):BasePreview(parent)
    {
        _renderer = cloudsPreviewCoverageCreateRenderer();
        _3d = true;

        _original_layer = layer;

        addToggle("3d", tr("Perspective"), true);
        configScaling(100.0, 1000.0, 20.0, 200.0);
    }
    ~PreviewCloudsCoverage()
    {
        rendererDelete(_renderer);
    }
protected:
    Color getColor(double x, double y)
    {
        return cloudsPreviewCoverageGetPixel(_renderer, x, y, scaling, _3d);
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
        cloudsPreviewCoverageBindLayer(_renderer, _original_layer);
    }

private:
    Renderer* _renderer;
    CloudsLayerDefinition* _original_layer;
    bool _3d;
};

class PreviewCloudsColor:public BasePreview
{
public:
    PreviewCloudsColor(QWidget* parent, CloudsLayerDefinition* layer):BasePreview(parent)
    {
        _original_layer = layer;

        _renderer = cloudsPreviewMaterialCreateRenderer();

        configScaling(0.5, 2.0, 0.1, 2.0);
    }

    ~PreviewCloudsColor()
    {
        rendererDelete(_renderer);
    }
protected:
    Color getColor(double x, double y)
    {
        return cloudsPreviewMaterialGetPixel(_renderer, x, y);
    }
    void updateData()
    {
        cloudsPreviewMaterialBindLayer(_renderer, _original_layer);
    }
private:
    Renderer* _renderer;
    CloudsLayerDefinition* _original_layer;
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

FormClouds::~FormClouds()
{
    CloudsDefinitionClass.destroy(_definition);
    cloudsGetLayerType().callback_delete(_layer);

    delete _previewCoverage;
    delete _previewColor;
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
