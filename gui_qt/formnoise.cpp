#include "formnoise.h"

#include "tools.h"

#include <QColor>
#include <QSlider>
#include <math.h>

#include "../lib_paysages/sky.h"
#include "../lib_paysages/shared/functions.h"
#include "../lib_paysages/shared/constants.h"

/**************** Previews ****************/
class PreviewLevel:public Preview
{
public:
    PreviewLevel(QWidget* parent, NoiseGenerator* noise):
        Preview(parent),
        _noise(noise)
    {
    }
protected:
    QColor getColor(double x, double y)
    {
        /*Vector3 eye = {0.0, 0.0, 0.0};
        Vector3 look;

        look.x = cos(M_PI * (x / 128.0 + 0.5)) * cos(M_PI * (y / 256.0));
        look.y = -sin(M_PI * (y / 256.0));
        look.z = sin(M_PI * (x / 128.0 + 0.5)) * cos(M_PI * (y / 256.0));

        return colorToQColor(skyGetColorCustom(eye, look, &_definition, NULL, NULL));*/
    }
private:
    NoiseGenerator* _noise;
};

class PreviewTotal:public Preview
{
public:
    PreviewTotal(QWidget* parent, NoiseGenerator* noise):
        Preview(parent),
        _noise(noise)
    {
    }
protected:
    QColor getColor(double x, double y)
    {
        if (y > noiseGet1DTotal(_noise, x))
        {
            return QColor(255, 255, 255);
        }
        else
        {
            return QColor(0, 0, 0);
        }
    }
private:
    NoiseGenerator* _noise;
};

/**************** Form ****************/
FormNoise::FormNoise(QWidget *parent, NoiseGenerator* noise):
    BaseForm(parent)
{
    /*_definition = skyCreateDefinition();*/

    previewLevel = new PreviewLevel(this, noise);
    addPreview(previewLevel, QString("Level preview"));
    previewTotal = new PreviewTotal(this, noise);
    addPreview(previewTotal, QString("Total preview"));

    /*addInputDouble("Day time", &_definition.daytime, 0.0, 1.0, 0.01, 0.1);*/

    revertConfig();
}

void FormNoise::revertConfig()
{
    /*skyCopyDefinition(skyGetDefinition(), &_definition);
    BaseForm::revertConfig();*/
}

void FormNoise::applyConfig()
{
    /*skySetDefinition(_definition);
    BaseForm::applyConfig();*/
}

void FormNoise::applyConfigPreview()
{
    /*skyValidateDefinition(&_definition);
    BaseForm::applyConfigPreview();*/
}
