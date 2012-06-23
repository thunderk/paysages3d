#include "formsky.h"

#include "tools.h"

#include <QColor>
#include <QSlider>
#include <math.h>

#include "../lib_paysages/sky.h"
#include "../lib_paysages/scenery.h"
#include "../lib_paysages/renderer.h"

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
        y -= 100.0 * scaling;
        if (y > 0.0)
        {
            return QColor(0, 0, 0);
        }
        else
        {
            Vector3 eye = {0.0, 0.0, 0.0};
            Vector3 look;

            look.x = 1.0;
            look.y = -y;
            look.z = x;

            return colorToQColor(skyGetColor(&_preview_definition, &_renderer, eye, look));
        }
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
        y -= 100.0 * scaling;
        if (y > 0.0)
        {
            return QColor(0, 0, 0);
        }
        else
        {
            Vector3 eye = {0.0, 0.0, 0.0};
            Vector3 look;

            look.x = -1.0;
            look.y = -y;
            look.z = -x;

            return colorToQColor(skyGetColor(&_preview_definition, &_renderer, eye, look));
        }
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
    BaseInput* input;
    
    _definition = skyCreateDefinition();

    previewWest = new PreviewSkyWest(this);
    addPreview(previewWest, QString(tr("West preview")));
    previewEast = new PreviewSkyEast(this);
    addPreview(previewEast, QString(tr("East preview")));

    addInputEnum(tr("Color model"), (int*)&_definition.model, QStringList(tr("Custom model")) << tr("Mixed Preetham/Shirley approximation"));
    addInputDouble(tr("Day time"), &_definition.daytime, 0.0, 1.0, 0.002, 0.1);
    addInputColor(tr("Sun color"), &_definition.sun_color);
    addInputDouble(tr("Sun radius"), &_definition.sun_radius, 0.0, 0.4, 0.004, 0.04);
    addInputDouble(tr("Sun halo radius"), &_definition.sun_halo_size, 0.0, 0.4, 0.004, 0.04);
    addInputCurve(tr("Sun halo profile"), _definition.sun_halo_profile, 0.0, 1.0, 0.0, 1.0);
    input = addInputBoolean(tr("Auto colors from daytime"), &_definition.model_custom.auto_from_daytime);
    input->setVisibilityCondition((int*)&_definition.model, SKY_MODEL_CUSTOM);
    input = addInputColor(tr("Zenith color"), &_definition.model_custom.zenith_color);
    input->setVisibilityCondition((int*)&_definition.model, SKY_MODEL_CUSTOM);
    input = addInputColor(tr("Haze color"), &_definition.model_custom.haze_color);
    input->setVisibilityCondition((int*)&_definition.model, SKY_MODEL_CUSTOM);
    input = addInputDouble(tr("Haze height"), &_definition.model_custom.haze_height, 0.0, 1.0, 0.01, 0.1);
    input->setVisibilityCondition((int*)&_definition.model, SKY_MODEL_CUSTOM);
    input = addInputDouble(tr("Haze smoothing"), &_definition.model_custom.haze_smoothing, 0.0, 1.0, 0.01, 0.1);
    input->setVisibilityCondition((int*)&_definition.model, SKY_MODEL_CUSTOM);
    input = addInputDouble(tr("Turbidity"), &_definition.model_preetham.turbidity, 1.8, 6.0, 0.05, 0.5);
    input->setVisibilityCondition((int*)&_definition.model, SKY_MODEL_PREETHAM);

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
