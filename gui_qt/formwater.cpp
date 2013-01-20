#include "basepreview.h"
#include "formwater.h"

#include <QColor>
#include <QSlider>
#include <math.h>

#include "../lib_paysages/tools/euclid.h"
#include "../lib_paysages/tools/lighting.h"
#include "../lib_paysages/renderer.h"
#include "../lib_paysages/scenery.h"
#include "../lib_paysages/water.h"
#include "tools.h"

static WaterDefinition _definition;

/**************** Previews ****************/
class PreviewWaterCoverage:public BasePreview
{
public:
    PreviewWaterCoverage(QWidget* parent):BasePreview(parent)
    {
        _renderer = terrainCreatePreviewRenderer();
        _water = waterCreateDefinition();
        _highlight_enabled = true;

        addOsd(QString("geolocation"));
        addToggle("highlight", tr("Coverage highlight"), true);

        configScaling(0.5, 200.0, 3.0, 50.0);
        configScrolling(-1000.0, 1000.0, 0.0, -1000.0, 1000.0, 0.0);
    }
protected:
    Color getColor(double x, double y)
    {
        double height;

        height = _renderer->terrain->getHeight(_renderer, x, -y, 1);
        if (height > _definition.height)
        {
            return terrainGetPreviewColor(_renderer, x, -y, scaling);
        }
        else
        {
            Vector3 location, look;
            Color base;

            location.x = x;
            location.y = _water.height;
            location.z = -y;

            look.x = 0.0;
            look.y = -1.0;
            look.z = 0.0;

            base = waterGetColor(&_water, _renderer, location, look);

            if (_highlight_enabled)
            {
                Color mask = {0.5, 0.5, 1.0, 0.5};
                colorMask(&base, &mask);
            }

            return base;
        }
    }
    void updateData()
    {
        waterCopyDefinition(&_definition, &_water);

        // TODO Do this only on full refresh
        TerrainDefinition* terrain = (TerrainDefinition*)TerrainDefinitionClass.create();
        sceneryGetTerrain(terrain);
        TerrainRendererClass.bind(_renderer, terrain);
        TerrainDefinitionClass.destroy(terrain);
    }
    void toggleChangeEvent(QString key, bool value)
    {
        if (key == "highlight")
        {
            _highlight_enabled = value;
            redraw();
        }
    }
private:
    Renderer* _renderer;
    WaterDefinition _water;
    bool _highlight_enabled;
};

class PreviewWaterColor:public BasePreview
{
public:
    PreviewWaterColor(QWidget* parent):BasePreview(parent)
    {
        _background = 0;
        _lighting_enabled = false;

        _water = waterCreateDefinition();

        _renderer = rendererCreate();
        _renderer->atmosphere->getLightingStatus = _getLightingStatus;
        _renderer->rayWalking = _rayWalking;
        _renderer->customData[0] = &_water;
        _renderer->customData[2] = this;

        configScaling(10.0, 1000.0, 10.0, 250.0);
        //configScrolling(-30.0, 30.0, 0.0, -20.0, 20.0, 0.0);

        addChoice("bg", tr("Background"), QStringList(tr("None")) << tr("Grid") << tr("Sinusoid"), 2);
        addToggle("light", tr("Lighting"), true);
    }
    int _background;
    bool _lighting_enabled;
protected:
    Color getColor(double x, double y)
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
            return _rayWalking(_renderer, eye, look, 0, 0, 0, 0).hit_color;
        }

        location.x = eye.x - look.x * eye.y / look.y;
        location.y = 0.0;
        location.z = eye.z - look.z * eye.y / look.y;

        if (location.z > 0.0)
        {
            return _rayWalking(_renderer, eye, look, 0, 0, 0, 0).hit_color;
        }

        return waterGetColor(&_water, _renderer, location, look);
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
    Renderer* _renderer;
    WaterDefinition _water;

    static RayCastingResult _rayWalking(Renderer* renderer, Vector3 location, Vector3 direction, int, int, int, int)
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
    static void _getLightingStatus(Renderer* renderer, LightStatus* status, Vector3, int)
    {
        LightDefinition light;
        PreviewWaterColor* preview = (PreviewWaterColor*)renderer->customData[2];
        light.color = COLOR_WHITE;
        light.direction.x = 0.0;
        light.direction.y = -0.4794;
        light.direction.z = 0.8776;
        light.altered = 0;
        if (preview->_lighting_enabled)
        {
            light.reflection = 1.0;
        }
        else
        {
            light.reflection = 0.0;
        }
        lightingPushLight(status, &light);
    }
};

/**************** Form ****************/
FormWater::FormWater(QWidget *parent):
    BaseForm(parent)
{
    addAutoPreset(tr("Lake surface"));
    addAutoPreset(tr("Standard sea"));

    _definition = waterCreateDefinition();

    previewCoverage = new PreviewWaterCoverage(this);
    previewColor = new PreviewWaterColor(this);
    addPreview(previewCoverage, tr("Coverage preview"));
    addPreview(previewColor, tr("Rendered preview"));

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
    addInputDouble(tr("Foam coverage"), &_definition.foam_coverage, 0.0, 1.0, 0.01, 0.1);
    addInputMaterial(tr("Foam material"), &_definition.foam_material);

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

void FormWater::autoPresetSelected(int preset)
{
    waterAutoPreset(&_definition, (WaterPreset)preset);
    BaseForm::autoPresetSelected(preset);
}

