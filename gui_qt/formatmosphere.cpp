#include "formatmosphere.h"

#include "tools.h"

#include <QColor>
#include <QSlider>
#include <math.h>

#include "../lib_paysages/atmosphere/public.h"
#include "../lib_paysages/scenery.h"
#include "../lib_paysages/renderer.h"

static AtmosphereDefinition* _definition;

/**************** Previews ****************/
class PreviewSkyEast:public BasePreview
{
public:
    PreviewSkyEast(QWidget* parent):
        BasePreview(parent)
    {
        _renderer = atmosphereCreatePreviewRenderer();

        configHdrToneMapping(true);
        configScaling(0.5, 5.0, 0.5, 2.5);
    }
protected:
    Color getColor(double x, double y)
    {
        return atmosphereGetPreview(_renderer, x, -y, -M_PI_2);
    }
    void updateData()
    {
        AtmosphereRendererClass.bind(_renderer, _definition);
    }
private:
    Renderer* _renderer;
};

class PreviewSkyWest:public BasePreview
{
public:
    PreviewSkyWest(QWidget* parent):
        BasePreview(parent)
    {
        _renderer = atmosphereCreatePreviewRenderer();

        configHdrToneMapping(true);
        configScaling(0.5, 5.0, 0.5, 2.5);
    }
protected:
    Color getColor(double x, double y)
    {
        return atmosphereGetPreview(_renderer, x, -y, M_PI_2);
    }
    void updateData()
    {
        AtmosphereRendererClass.bind(_renderer, _definition);
    }
private:
    Renderer* _renderer;
};

/**************** Form ****************/
FormAtmosphere::FormAtmosphere(QWidget *parent):
    BaseForm(parent)
{
    addAutoPreset(tr("Clear day"));
    addAutoPreset(tr("Clear sunset"));
    addAutoPreset(tr("Hazy morning"));
    addAutoPreset(tr("Foggy"));
    addAutoPreset(tr("Stormy"));

    _definition = (AtmosphereDefinition*)AtmosphereDefinitionClass.create();

    previewWest = new PreviewSkyWest(this);
    addPreview(previewWest, QString(tr("West preview")));
    previewEast = new PreviewSkyEast(this);
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

void FormAtmosphere::revertConfig()
{
    sceneryGetAtmosphere(_definition);
    BaseForm::revertConfig();
}

void FormAtmosphere::applyConfig()
{
    scenerySetAtmosphere(_definition);
    BaseForm::applyConfig();
}

void FormAtmosphere::configChangeEvent()
{
    AtmosphereDefinitionClass.validate(_definition);
    BaseForm::configChangeEvent();
}

void FormAtmosphere::autoPresetSelected(int preset)
{
    atmosphereAutoPreset(_definition, (AtmospherePreset)preset);
    BaseForm::autoPresetSelected(preset);
}
