#include "formsky.h"

#include "tools.h"

#include <QColor>
#include <QSlider>
#include <math.h>

#include "../lib_paysages/sky.h"
#include "../lib_paysages/shared/functions.h"
#include "../lib_paysages/shared/constants.h"

static SkyDefinition _definition;

/**************** Previews ****************/
class PreviewEast:public Preview
{
public:
    PreviewEast(QWidget* parent):
        Preview(parent)
    {
    }
protected:
    QColor getColor(double x, double y)
    {
        Vector3 eye = {0.0, 0.0, 0.0};
        Vector3 look;

        look.x = 100.0;
        look.y = -y;
        look.z = x;

        return colorToQColor(skyGetColorCustom(eye, look, &_definition, NULL, NULL));
    }
};

class PreviewWest:public Preview
{
public:
    PreviewWest(QWidget* parent):
        Preview(parent)
    {
    }
protected:
    QColor getColor(double x, double y)
    {
        Vector3 eye = {0.0, 0.0, 0.0};
        Vector3 look;

        look.x = -100.0;
        look.y = -y;
        look.z = -x;

        return colorToQColor(skyGetColorCustom(eye, look, &_definition, NULL, NULL));
    }
};

/**************** Form ****************/
FormSky::FormSky(QWidget *parent):
    BaseForm(parent)
{
    _definition = skyCreateDefinition();

    previewWest = new PreviewWest(this);
    addPreview(previewWest, QString("West preview"));
    previewEast = new PreviewEast(this);
    addPreview(previewEast, QString("East preview"));

    addInputDouble("Day time", &_definition.daytime, 0.0, 1.0, 0.01, 0.1);
    addInputColorGradation("Sun color", &_definition.sun_color);
    addInputDouble("Sun radius", &_definition.sun_radius, 0.0, 0.3, 0.01, 0.03);
    addInputColorGradation("Zenith color", &_definition.zenith_color);
    addInputColorGradation("Haze color", &_definition.haze_color);
    addInputDouble("Haze height", &_definition.haze_height, 0.0, 1.0, 0.01, 0.1);
    addInputDouble("Haze smoothing", &_definition.haze_smoothing, 0.0, 1.0, 0.01, 0.1);

    revertConfig();
}

void FormSky::revertConfig()
{
    skyCopyDefinition(skyGetDefinition(), &_definition);
    BaseForm::revertConfig();
}

void FormSky::applyConfig()
{
    skySetDefinition(_definition);
    BaseForm::applyConfig();
}

void FormSky::applyConfigPreview()
{
    skyValidateDefinition(&_definition);
    BaseForm::applyConfigPreview();
}
