#include "formclouds.h"

#include "clouds/clo_preview.h"
#include "tools/color.h"
#include "tools/euclid.h"
#include "Scenery.h"
#include "BasePreview.h"
#include "renderer.h"
#include "CloudsDefinition.h"
#include "CloudLayerDefinition.h"

#include "tools.h"

/**************** Previews ****************/
class PreviewCloudsCoverage:public BasePreview
{
public:
    PreviewCloudsCoverage(QWidget* parent, CloudLayerDefinition* layer):BasePreview(parent)
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
    CloudLayerDefinition* _original_layer;
    bool _3d;
};

class PreviewCloudsColor:public BasePreview
{
public:
    PreviewCloudsColor(QWidget* parent, CloudLayerDefinition* layer):BasePreview(parent)
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
    CloudLayerDefinition* _original_layer;
};

/**************** Form ****************/
FormClouds::FormClouds(QWidget *parent):
    BaseFormLayer(parent)
{
    addAutoPreset(tr("Cirrus"));
    addAutoPreset(tr("Cumulus"));
    addAutoPreset(tr("Stratocumulus"));
    addAutoPreset(tr("Stratus"));

    _definition = new CloudsDefinition(NULL);
    _layer = new CloudLayerDefinition(NULL);

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
    addInputMaterial(tr("Material"), _layer->material);
    addInputDouble(tr("Hardness to light"), &_layer->hardness, 0.0, 1.0, 0.01, 0.1);
    addInputDouble(tr("Transparency depth"), &_layer->transparencydepth, 0.0, 10.0, 0.1, 1.0);
    addInputDouble(tr("Light traversal depth"), &_layer->lighttraversal, 0.0, 10.0, 0.1, 1.0);
    addInputDouble(tr("Minimum lighting"), &_layer->minimumlight, 0.0, 1.0, 0.01, 0.1);

    setLayers(_definition);
}

FormClouds::~FormClouds()
{
    delete _layer;
    delete _definition;

    delete _previewCoverage;
    delete _previewColor;
}

void FormClouds::revertConfig()
{
    Scenery::getCurrent()->getClouds(_definition);
    BaseFormLayer::revertConfig();
}

void FormClouds::applyConfig()
{
    BaseFormLayer::applyConfig();
    Scenery::getCurrent()->setClouds(_definition);
}

void FormClouds::layerReadCurrentFrom(void* layer_definition)
{
    ((CloudLayerDefinition*)layer_definition)->copy(_layer);
}

void FormClouds::layerWriteCurrentTo(void* layer_definition)
{
    _layer->copy((CloudLayerDefinition*)layer_definition);
}

void FormClouds::autoPresetSelected(int preset)
{
    _layer->applyPreset((CloudLayerDefinition::CloudsLayerPreset)preset);
    BaseForm::autoPresetSelected(preset);
}
