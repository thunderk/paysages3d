#include "basepreview.h"
#include "formwater.h"

#include <QColor>
#include <QSlider>
#include <math.h>

#include "../lib_paysages/euclid.h"
#include "../lib_paysages/lighting.h"
#include "../lib_paysages/renderer.h"
#include "../lib_paysages/scenery.h"
#include "../lib_paysages/terrain.h"
#include "../lib_paysages/water.h"
#include "tools.h"

static WaterDefinition _definition;

/**************** Previews ****************/
class PreviewWaterCoverage:public BasePreview
{
public:
    PreviewWaterCoverage(QWidget* parent):BasePreview(parent)
    {
        _water = waterCreateDefinition();
        _terrain = terrainCreateDefinition();
        
        addOsd(QString("geolocation"));
        
        configScaling(0.5, 200.0, 3.0, 50.0);
        configScrolling(-1000.0, 1000.0, 0.0, -1000.0, 1000.0, 0.0);
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
            return colorToQColor(_definition.material.base);
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

class PreviewWaterColor:public BasePreview
{
public:
    PreviewWaterColor(QWidget* parent):BasePreview(parent)
    {
        LightDefinition light;
        
        _background = 0;
        _lighting_enabled = false;
        
        _water = waterCreateDefinition();
        
        _lighting = lightingCreateDefinition();
        light.color = COLOR_WHITE;
        light.direction.x = 0.0;
        light.direction.y = -0.4794;
        light.direction.z = 0.8776;
        light.filtered = 0;
        light.masked = 0;
        light.reflection = 1.0;
        lightingAddLight(&_lighting, light);
        lightingValidateDefinition(&_lighting);

        _renderer = rendererCreate();
        _renderer.rayWalking = _rayWalking;
        _renderer.getLightStatus = _getLightStatus;
        _renderer.applyLightStatus = _applyLightStatus;
        _renderer.customData[0] = &_water;
        _renderer.customData[1] = &_lighting;
        _renderer.customData[2] = this;
        
        configScaling(10.0, 1000.0, 10.0, 250.0);
        //configScrolling(-30.0, 30.0, 0.0, -20.0, 20.0, 0.0);

        addChoice("bg", tr("Background"), QStringList(tr("None")) << tr("Grid") << tr("Sinusoid"), 2);
        addToggle("light", tr("Lighting"), true);
    }
    int _background;
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
    void choiceChangeEvent(const QString& key, int position)
    {
        if (key == "bg")
        {
            _background = position;
            redraw();
        }
    }
    void toggleChangeEvent(QString key, bool value)
    {
        if (key == "light")
        {
            _lighting_enabled = value;
            redraw();
        }
    }

private:
    Renderer _renderer;
    WaterDefinition _water;
    LightingDefinition _lighting;
    bool _lighting_enabled;
    
    static RayCastingResult _rayWalking(Renderer* renderer, Vector3 location, Vector3 direction, int terrain, int water, int sky, int clouds)
    {
        RayCastingResult result;
        PreviewWaterColor* preview = (PreviewWaterColor*)renderer->customData[2];
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

            switch (preview->_background)
            {
            case 1:
                result.hit_color = (((int)ceil(x * 0.2) % 2 == 0) ^ ((int)ceil(y * 0.2 - 0.5) % 2 == 0)) ? COLOR_WHITE : COLOR_BLACK;
                break;
            case 2:
                result.hit_color = (y * 0.1 > x * 0.03 + sin(x - M_PI_2)) ? COLOR_WHITE : COLOR_BLACK;
                break;
            default:
                result.hit_color = COLOR_WHITE;
            }
            result.hit_location.x = x;
            result.hit_location.y = y;
            result.hit_location.z = 0.0;
        }

        return result;
    }
    static Color _applyLightStatus(Renderer* renderer, LightStatus* status, Vector3 location, Vector3 normal, SurfaceMaterial material)
    {
        if (((PreviewWaterColor*)renderer->customData[2])->_lighting_enabled)
        {
            return lightingApplyStatusToSurface(renderer, status, location, normal, material);
        }
        else
        {
            return material.base;
        }
    }
    static void _getLightStatus(Renderer* renderer, LightStatus* status, Vector3 location)
    {
        lightingGetStatus((LightingDefinition*)renderer->customData[1], renderer, location, status);
    }
};

/**************** Form ****************/
FormWater::FormWater(QWidget *parent):
    BaseForm(parent)
{
    _definition = waterCreateDefinition();

    previewCoverage = new PreviewWaterCoverage(this);
    previewColor = new PreviewWaterColor(this);
    addPreview(previewCoverage, QString(tr("Coverage preview")));
    addPreview(previewColor, QString(tr("Rendered preview")));

    addInputDouble(tr("Height"), &_definition.height, -10.0, 10.0, 0.1, 1.0);
    addInputMaterial(tr("Surface material"), &_definition.material);
    addInputColor(tr("Depth color"), &_definition.depth_color);
    addInputDouble(tr("Transparency"), &_definition.transparency, 0.0, 1.0, 0.001, 0.1);
    addInputDouble(tr("Reflection"), &_definition.reflection, 0.0, 1.0, 0.001, 0.1);
    addInputDouble(tr("Transparency distance"), &_definition.transparency_depth, 0.0, 20.0, 0.1, 1.0);
    addInputDouble(tr("Light-through distance"), &_definition.lighting_depth, 0.0, 20.0, 0.1, 1.0);
    addInputDouble(tr("Waves scaling"), &_definition.scaling, 0.02, 2.0, 0.02, 0.2);
    addInputDouble(tr("Waves height"), &_definition.waves_height, 0.0, 2.0, 0.02, 0.2);
    addInputDouble(tr("Waves detail"), &_definition.detail_height, 0.0, 0.3, 0.003, 0.03);
    addInputDouble(tr("Waves turbulence"), &_definition.turbulence, 0.0, 0.5, 0.005, 0.05);

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

void FormWater::configChangeEvent()
{
    waterValidateDefinition(&_definition);
    BaseForm::configChangeEvent();
}
