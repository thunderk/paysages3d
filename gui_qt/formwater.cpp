#include "preview.h"
#include "formwater.h"
#include "tools.h"
#include <QColor>
#include <QSlider>
#include <math.h>

#include "../lib_paysages/terrain.h"
#include "../lib_paysages/water.h"
#include "../lib_paysages/scenery.h"
#include "../lib_paysages/renderer.h"
#include "../lib_paysages/shared/functions.h"
#include "../lib_paysages/shared/constants.h"

static WaterDefinition _definition;

/**************** Previews ****************/
class PreviewWaterCoverage:public Preview
{
public:
    PreviewWaterCoverage(QWidget* parent):Preview(parent)
    {
        _water = waterCreateDefinition();
        _terrain = terrainCreateDefinition();
    }
protected:
    QColor getColor(double x, double y)
    {
        double height;

        height = terrainGetHeight(&_terrain, x, y);
        if (height > _definition.height)
        {
            height = terrainGetHeightNormalized(&_terrain, x, y);
            return QColor((int)(255.0 * height), (int)(255.0 * height), (int)(255.0 * height));
        }
        else
        {
            return colorToQColor(_definition.main_color);
        }
    }
    void updateData()
    {
        waterCopyDefinition(&_definition, &_water);
        sceneryGetTerrain(&_terrain);
    }
private:
    WaterDefinition _water;
    TerrainDefinition _terrain;
};

class PreviewWaterColor:public Preview
{
public:
    PreviewWaterColor(QWidget* parent):Preview(parent)
    {
        _water = waterCreateDefinition();

        _renderer = rendererGetFake();
        _renderer.rayWalking = _rayWalking;
        // TODO Lighting
    }
protected:
    QColor getColor(double x, double y)
    {
        Vector3 eye, look, location;

        eye.x = 0.0;
        eye.y = scaling;
        eye.z = -10.0 * scaling;
        look.x = x * 0.01 / scaling;
        look.y = -y * 0.01 / scaling - 0.3;
        look.z = 1.0;
        look = v3Normalize(look);

        if (look.y > -0.0001)
        {
            return colorToQColor(_rayWalking(&_renderer, eye, look, 0, 0, 0, 0).hit_color);
        }

        location.x = eye.x - look.x * eye.y / look.y;
        location.y = 0.0;
        location.z = eye.z - look.z * eye.y / look.y;

        if (location.z > 0.0)
        {
            return colorToQColor(_rayWalking(&_renderer, eye, look, 0, 0, 0, 0).hit_color);
        }

        return colorToQColor(waterGetColor(&_water, &_renderer, location, look));
    }
    void updateData()
    {
        waterCopyDefinition(&_definition, &_water);
        _water.height = 0.0;
    }

private:
    Renderer _renderer;
    WaterDefinition _water;
    static RayCastingResult _rayWalking(Renderer* renderer, Vector3 location, Vector3 direction, int terrain, int water, int sky, int clouds)
    {
        RayCastingResult result;
        double x, y;

        result.hit = 1;
        if (direction.z < 0.0001)
        {
            result.hit_color = COLOR_WHITE;
            result.hit_location = location;
        }
        else
        {
            x = location.x + direction.x * (0.0 - location.z) / direction.z;
            y = location.y + direction.y * (0.0 - location.z) / direction.z;

            if (((int)ceil(x * 0.2) % 2 == 0) ^ ((int)ceil(y * 0.2 - 0.5) % 2 == 0))
            {
                result.hit_color = COLOR_WHITE;
            }
            else
            {
                result.hit_color = COLOR_GREY;
            }
            result.hit_location.x = x;
            result.hit_location.y = y;
            result.hit_location.z = 0.0;
        }

        return result;
    }
};

/**************** Form ****************/
FormWater::FormWater(QWidget *parent):
    BaseForm(parent)
{
    _definition = waterCreateDefinition();

    previewCoverage = new PreviewWaterCoverage(this);
    previewColor = new PreviewWaterColor(this);
    addPreview(previewCoverage, QString("Coverage preview"));
    addPreview(previewColor, QString("Color preview"));

    addInputDouble("Height", &_definition.height, -10.0, 10.0, 0.1, 1.0);
    addInputColor("Surface color", &_definition.main_color);
    addInputDouble("Transparency", &_definition.transparency, 0.0, 1.0, 0.001, 0.1);
    addInputDouble("Reflection", &_definition.reflection, 0.0, 1.0, 0.001, 0.1);
    addInputColor("Depth color", &_definition.depth_color);
    addInputDouble("Depth filtering", &_definition.transparency_depth, 0.0, 100.0, 0.5, 5.0);
    addInputNoise("Waves noise", _definition.waves_noise);
    addInputDouble("Waves height", &_definition.waves_noise_height, 0.0, 0.1, 0.001, 0.01);
    addInputDouble("Waves scaling", &_definition.waves_noise_scale, 0.01, 1.0, 0.01, 0.1);

    revertConfig();
}

void FormWater::revertConfig()
{
    sceneryGetWater(&_definition);
    BaseForm::revertConfig();
}

void FormWater::applyConfig()
{
    scenerySetWater(&_definition);
    BaseForm::applyConfig();
}
