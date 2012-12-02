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
        _renderer = rendererCreate();

        configScaling(0.5, 5.0, 0.5, 2.5);
    }
protected:
    QColor getColor(double x, double y)
    {
        y -= 100.0 * scaling;
        if (y > 0.0)
        {
            return QColor(0, 0, 0);
        }
        else
        {
            Vector3 look;

            look.x = 1.0;
            look.y = -y;
            look.z = x;

            return colorToQColor(_renderer.atmosphere->getSkyColor(&_renderer, look));
        }
    }
    void updateData()
    {
        AtmosphereRendererClass.bind(_renderer.atmosphere, _definition);
    }
private:
    Renderer _renderer;
};

class PreviewSkyWest:public BasePreview
{
public:
    PreviewSkyWest(QWidget* parent):
        BasePreview(parent)
    {
        _renderer = rendererCreate();

        configScaling(0.5, 5.0, 0.5, 2.5);
    }
protected:
    QColor getColor(double x, double y)
    {
        y -= 100.0 * scaling;
        if (y > 0.0)
        {
            return QColor(0, 0, 0);
        }
        else
        {
            Vector3 look;

            look.x = -1.0;
            look.y = -y;
            look.z = -x;

            return colorToQColor(_renderer.atmosphere->getSkyColor(&_renderer, look));
        }
    }
    void updateData()
    {
        AtmosphereRendererClass.bind(_renderer.atmosphere, _definition);
    }
private:
    Renderer _renderer;
};

/**************** Form ****************/
FormAtmosphere::FormAtmosphere(QWidget *parent):
    BaseForm(parent)
{
    BaseInput* input;

    _definition = (AtmosphereDefinition*)AtmosphereDefinitionClass.create();

    previewWest = new PreviewSkyWest(this);
    addPreview(previewWest, QString(tr("West preview")));
    previewEast = new PreviewSkyEast(this);
    addPreview(previewEast, QString(tr("East preview")));

    addInputEnum(tr("Color model"), (int*)&_definition->model, QStringList(tr("Preetham/Shirley analytic model")) << tr("Bruneton/Neyret precomputed model"));
    addInputDouble(tr("Day time"), &_definition->daytime, 0.14, 0.86, 0.002, 0.1);
    addInputColor(tr("Sun color"), &_definition->sun_color);
    addInputDouble(tr("Sun radius"), &_definition->sun_radius, 0.0, 0.4, 0.004, 0.04);
    addInputDouble(tr("Sun halo radius"), &_definition->sun_halo_size, 0.0, 0.4, 0.004, 0.04);
    addInputCurve(tr("Sun halo profile"), _definition->sun_halo_profile, 0.0, 1.0, 0.0, 1.0, tr("Distance to center of the sun"), tr("Light influence (halo opacity)"));
    addInputDouble(tr("Influence of skydome on lighting"), &_definition->dome_lighting, 0.0, 2.0, 0.01, 0.1);
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
