#include "formclouds.h"

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
    addInputDouble(tr("Lower altitude"), &_layer->altitude, 0.0, 1.0, 0.01, 0.1);
    addInputDouble(tr("Scaling"), &_layer->scaling, 0.0, 1.0, 0.01, 0.1);
    addInputDouble(tr("Coverage"), &_layer->coverage, 0.0, 1.0, 0.01, 0.1);

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
