#include "formsky.h"

#include "tools.h"

#include <QColor>
#include <QSlider>
#include <math.h>

#include "../lib_paysages/sky.h"
#include "../lib_paysages/scenery.h"
#include "../lib_paysages/renderer.h"
#include "../lib_paysages/shared/constants.h"

static SkyDefinition _definition;

/**************** Previews ****************/
class PreviewSkyEast:public BasePreview
{
public:
    PreviewSkyEast(QWidget* parent):
        BasePreview(parent)
    {
        _renderer = rendererCreate();
        _preview_definition = skyCreateDefinition();
        
        configScaling(0.5, 5.0, 0.5, 2.5);
    }
protected:
    QColor getColor(double x, double y)
    {
        Vector3 eye = {0.0, 0.0, 0.0};
        Vector3 look;

        look.x = 1.0;
        look.y = -y;
        look.z = x;

        return colorToQColor(skyGetColor(&_preview_definition, &_renderer, eye, look));
    }
    void updateData()
    {
        skyCopyDefinition(&_definition, &_preview_definition);
    }
private:
    Renderer _renderer;
    SkyDefinition _preview_definition;
};

class PreviewSkyWest:public BasePreview
{
public:
    PreviewSkyWest(QWidget* parent):
        BasePreview(parent)
    {
        _renderer = rendererCreate();
        _preview_definition = skyCreateDefinition();
        
        configScaling(0.5, 5.0, 0.5, 2.5);
    }
protected:
    QColor getColor(double x, double y)
    {
        Vector3 eye = {0.0, 0.0, 0.0};
        Vector3 look;

        look.x = -1.0;
        look.y = -y;
        look.z = -x;

        return colorToQColor(skyGetColor(&_preview_definition, &_renderer, eye, look));
    }
    void updateData()
    {
        skyCopyDefinition(&_definition, &_preview_definition);
    }
private:
    Renderer _renderer;
    SkyDefinition _preview_definition;
};

/**************** Form ****************/
FormSky::FormSky(QWidget *parent):
    BaseForm(parent)
{
    _definition = skyCreateDefinition();

    previewWest = new PreviewSkyWest(this);
    addPreview(previewWest, QString(tr("West preview")));
    previewEast = new PreviewSkyEast(this);
    addPreview(previewEast, QString(tr("East preview")));

    addInputDouble(tr("Day time"), &_definition.daytime, 0.0, 1.0, 0.01, 0.1);
    addInputColorGradation(tr("Sun color"), _definition.sun_color);
    addInputDouble(tr("Sun radius"), &_definition.sun_radius, 0.0, 0.3, 0.01, 0.03);
    addInputColorGradation(tr("Zenith color"), _definition.zenith_color);
    addInputColorGradation(tr("Haze color"), _definition.haze_color);
    addInputDouble(tr("Haze height"), &_definition.haze_height, 0.0, 1.0, 0.01, 0.1);
    addInputDouble(tr("Haze smoothing"), &_definition.haze_smoothing, 0.0, 1.0, 0.01, 0.1);

    revertConfig();
}

void FormSky::revertConfig()
{
    sceneryGetSky(&_definition);
    BaseForm::revertConfig();
}

void FormSky::applyConfig()
{
    scenerySetSky(&_definition);
    BaseForm::applyConfig();
}

void FormSky::configChangeEvent()
{
    skyValidateDefinition(&_definition);
    BaseForm::configChangeEvent();
}
