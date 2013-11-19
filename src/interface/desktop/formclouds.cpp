#include "formclouds.h"

#include "tools/euclid.h"
#include "RenderingScenery.h"
#include "BasePreview.h"
#include "renderer.h"
#include "CloudsDefinition.h"
#include "CloudLayerDefinition.h"
#include "CloudsCoveragePreviewRenderer.h"
#include "CloudsAspectPreviewRenderer.h"

#include "tools.h"

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

    _previewCoverageRenderer = new CloudsCoveragePreviewRenderer(_layer);
    _previewCoverage = new BasePreview(parent);
    addPreview(_previewCoverage, tr("Layer coverage (no lighting)"));
    _previewCoverage->setRenderer(_previewCoverageRenderer);

    _previewColorRenderer = new CloudsAspectPreviewRenderer(_layer);
    _previewColor = new BasePreview(parent);
    addPreview(_previewColor, tr("Appearance"));
    _previewColor->setRenderer(_previewColorRenderer);

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
    delete _previewCoverageRenderer;

    delete _previewColor;
    delete _previewColorRenderer;
}

void FormClouds::revertConfig()
{
    RenderingScenery::getCurrent()->getClouds(_definition);
    BaseFormLayer::revertConfig();
}

void FormClouds::applyConfig()
{
    BaseFormLayer::applyConfig();
    RenderingScenery::getCurrent()->setClouds(_definition);
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
