#include "formatmosphere.h"

#include "tools.h"

#include <QColor>
#include <QSlider>
#include <cmath>

#include "AtmosphereColorPreviewRenderer.h"
#include "RenderingScenery.h"
#include "BasePreview.h"
#include "AtmosphereDefinition.h"
#include "renderer.h"

static AtmosphereDefinition* _definition;

/**************** Form ****************/
FormAtmosphere::FormAtmosphere(QWidget *parent):
    BaseForm(parent)
{
    addAutoPreset(tr("Clear day"));
    addAutoPreset(tr("Clear sunset"));
    addAutoPreset(tr("Hazy morning"));
    addAutoPreset(tr("Foggy"));
    addAutoPreset(tr("Stormy"));

    _definition = new AtmosphereDefinition(NULL);

    previewWest = new BasePreview(this);
    previewWestRenderer = new AtmosphereColorPreviewRenderer(_definition, M_PI / 2.0);
    previewWest->setRenderer(previewWestRenderer);
    addPreview(previewWest, QString(tr("West preview")));

    previewEast = new BasePreview(this);
    previewEastRenderer = new AtmosphereColorPreviewRenderer(_definition, -M_PI / 2.0);
    previewEast->setRenderer(previewEastRenderer);
    addPreview(previewEast, QString(tr("East preview")));

    //addInputEnum(tr("Color model"), (int*)&_definition->model, QStringList(tr("Simplified model (with weather)")) << tr("Complex model"));
    addInputInt(tr("Day time (hour)"), &_definition->hour, 0, 23, 1, 10);
    addInputInt(tr("Day time (minute)"), &_definition->minute, 0, 59, 1, 10);
    //addInputColor(tr("Sun color"), &_definition->sun_color);
    addInputDouble(tr("Sun radius"), &_definition->sun_radius, 0.0, 5.0, 0.05, 0.5);
    //addInputDouble(tr("Influence of skydome on lighting"), &_definition->dome_lighting, 0.0, 2.0, 0.01, 0.1);
    addInputDouble(tr("Humidity"), &_definition->humidity, 0.0, 1.0, 0.01, 0.1);

    revertConfig();
}

FormAtmosphere::~FormAtmosphere()
{
    delete previewWest;
    delete previewWestRenderer;
    delete previewEast;
    delete previewEastRenderer;
}

void FormAtmosphere::revertConfig()
{
    RenderingScenery::getCurrent()->getAtmosphere(_definition);
    BaseForm::revertConfig();
}

void FormAtmosphere::applyConfig()
{
    RenderingScenery::getCurrent()->setAtmosphere(_definition);
    BaseForm::applyConfig();
}

void FormAtmosphere::configChangeEvent()
{
    _definition->validate();
    BaseForm::configChangeEvent();
}

void FormAtmosphere::autoPresetSelected(int preset)
{
    _definition->applyPreset((AtmosphereDefinition::AtmospherePreset)preset);
    BaseForm::autoPresetSelected(preset);
}
