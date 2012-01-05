#include "preview.h"
#include "formwater.h"
#include <QColor>
#include <QSlider>
#include <math.h>

#include "../lib_paysages/terrain.h"
#include "../lib_paysages/water.h"
#include "../lib_paysages/shared/functions.h"
#include "../lib_paysages/shared/constants.h"

static WaterDefinition _definition;

/**************** Previews ****************/
class PreviewCoverage:public Preview
{
public:
    PreviewCoverage(QWidget* parent):
        Preview(parent)
    {
    }
protected:
    QColor getColor(double x, double y)
    {
        double height;

        height = terrainGetHeight(x, y);
        if (height > _definition.height)
        {
            height = terrainGetHeightNormalized(x, y);
            return QColor((int)(255.0 * height), (int)(255.0 * height), (int)(255.0 * height));
        }
        else
        {
            return QColor(_definition.main_color.r * 255.0, _definition.main_color.g * 255.0, _definition.main_color.b * 255.0);
        }
    }
};

class PreviewColor:public Preview
{
public:
    PreviewColor(QWidget* parent):
        Preview(parent)
    {
    }
protected:
    QColor getColor(double x, double y)
    {
        Vector3 eye, look, location;
        WaterDefinition definition;
        WaterEnvironment environment;
        WaterQuality quality;
        Color result;

        eye.x = 0.0;
        eye.y = scaling;
        eye.z = -10.0 * scaling;
        look.x = x * 0.01 / scaling;
        look.y = -y * 0.01 / scaling - 0.3;
        look.z = 1.0;
        look = v3Normalize(look);

        if (look.y > -0.0001)
        {
            result = this->rayCastFromWater(eye, look).hit_color;
            return QColor(result.r * 255.0, result.g * 255.0, result.b * 255.0);
        }

        location.x = eye.x - look.x * eye.y / look.y;
        location.y = 0.0;
        location.z = eye.z - look.z * eye.y / look.y;

        if (location.z > 0.0)
        {
            result = this->rayCastFromWater(eye, look).hit_color;
            return QColor(result.r * 255.0, result.g * 255.0, result.b * 255.0);
        }

        definition = _definition;
        definition.height = 0.0;
        environment.reflection_function = (RayCastingFunction)(&this->rayCastFromWater);
        environment.refraction_function = (RayCastingFunction)(&this->rayCastFromWater);
        environment.toggle_fog = 0;
        environment.toggle_shadows = 0;
        quality.force_detail = 0.0001;

        result = waterGetColorCustom(location, look, &definition, &quality, &environment).final;
        return QColor(result.r * 255.0, result.g * 255.0, result.b * 255.0);
    }

private:
    static RayCastingResult rayCastFromWater(Vector3 start, Vector3 direction)
    {
        RayCastingResult result;
        double x, y;

        result.hit = 1;
        if (direction.z < 0.0001)
        {
            result.hit_color = COLOR_WHITE;
        }
        else
        {
            x = start.x + direction.x * (0.0 - start.z) / direction.z;
            y = start.y + direction.y * (0.0 - start.z) / direction.z;

            if (((int)ceil(x * 0.2) % 2 == 0) ^ ((int)ceil(y * 0.2 - 0.5) % 2 == 0))
            {
                result.hit_color = COLOR_WHITE;
            }
            else
            {
                result.hit_color = COLOR_GREY;
            }
        }
        /* TODO hit_location */

        return result;
    }
};

/**************** Form ****************/
FormWater::FormWater(QWidget *parent) :
    BaseForm(parent)
{
    _definition = waterCreateDefinition();

    previewCoverage = new PreviewCoverage(this);
    previewColor = new PreviewColor(this);
    addPreview(previewCoverage, QString("Coverage preview"));
    addPreview(previewColor, QString("Color preview"));

    addInputDouble("Height", &_definition.height, -20.0, 20.0, 0.1, 1.0);
    addInputColor("Surface color", &_definition.main_color);
    addInputDouble("Transparency", &_definition.transparency, 0.0, 1.0, 0.001, 0.1);
    addInputDouble("Reflection", &_definition.reflection, 0.0, 1.0, 0.001, 0.1);
    addInputColor("Depth color", &_definition.depth_color);
    addInputDouble("Depth filtering", &_definition.transparency_depth, 0.0, 100.0, 0.5, 5.0);

    revertConfig();
}

void FormWater::revertConfig()
{
    waterCopyDefinition(waterGetDefinition(), &_definition);
    BaseForm::revertConfig();
}

/*void FormWater::applyConfig()
{
    waterSetDefinition(_definition);
    //guiUpdate();
}*/
