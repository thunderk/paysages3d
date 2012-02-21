#include "formatmosphere.h"

#include "tools.h"
#include "../lib_paysages/atmosphere.h"
#include "../lib_paysages/scenery.h"
#include "../lib_paysages/euclid.h"
#include "../lib_paysages/color.h"

static AtmosphereDefinition _definition;

/**************** Previews ****************/
class PreviewAtmosphereColor:public BasePreview
{
public:
    PreviewAtmosphereColor(QWidget* parent):
        BasePreview(parent)
    {
        _renderer = rendererCreate();
        _preview_definition = atmosphereCreateDefinition();
    }
protected:
    QColor getColor(double x, double y)
    {
        Vector3 eye, look, location;

        eye.x = 0.0;
        eye.y = scaling * 5.0;
        eye.z = -10.0 * scaling;
        _renderer.camera_location = eye;
        look.x = x * 0.01 / scaling;
        look.y = -y * 0.01 / scaling - 0.3;
        look.z = 1.0;
        look = v3Normalize(look);

        if (look.y > -0.0001)
        {
            return colorToQColor(COLOR_BLUE);
        }

        location.x = eye.x - look.x * eye.y / look.y;
        location.y = 0.0;
        location.z = eye.z - look.z * eye.y / look.y;

        return colorToQColor(atmosphereApply(&_preview_definition, &_renderer, location, COLOR_BLACK));
    }
    void updateData()
    {
        atmosphereCopyDefinition(&_definition, &_preview_definition);
    }
private:
    Renderer _renderer;
    AtmosphereDefinition _preview_definition;
};

/**************** Form ****************/
FormAtmosphere::FormAtmosphere(QWidget *parent):
    BaseForm(parent)
{
    _definition = atmosphereCreateDefinition();

    previewColor = new PreviewAtmosphereColor(this);
    addPreview(previewColor, QString("Color preview"));

    addInputDouble("Start distance", &_definition.distance_near, -500.0, 500.0, 5.0, 50.0);
    addInputDouble("End distance", &_definition.distance_far, -500.0, 500.0, 5.0, 50.0);
    addInputDouble("Masking power", &_definition.full_mask, 0.0, 1.0, 0.01, 0.1);
    addInputBoolean("Lock color on haze", &_definition.auto_lock_on_haze);
    addInputColor("Color", &_definition.color);

    revertConfig();
}

void FormAtmosphere::revertConfig()
{
    sceneryGetAtmosphere(&_definition);
    BaseForm::revertConfig();
}

void FormAtmosphere::applyConfig()
{
    scenerySetAtmosphere(&_definition);
    BaseForm::applyConfig();
}

void FormAtmosphere::configChangeEvent()
{
    atmosphereValidateDefinition(&_definition);
    BaseForm::configChangeEvent();
}
