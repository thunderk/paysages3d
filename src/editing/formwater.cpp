#include "formwater.h"

#include <QColor>
#include <QSlider>
#include <cmath>

#include "tools/euclid.h"
#include "tools/lighting.h"
#include "renderer.h"
#include "water/public.h"
#include "terrain/public.h"
#include "atmosphere/public.h"
#include "tools.h"
#include "Scenery.h"
#include "BasePreview.h"
#include "camera.h"
#include "WaterDefinition.h"

static WaterDefinition* _definition;

/**************** Previews ****************/
class PreviewWaterCoverage : public BasePreview
{
public:

    PreviewWaterCoverage(QWidget* parent) : BasePreview(parent)
    {
        _renderer = rendererCreate();
        waterAlterPreviewCoverageRenderer(_renderer);
        _highlight_enabled = true;

        addOsd(QString("geolocation"));
        addToggle("highlight", tr("Coverage highlight"), true);

        configScaling(20.0, 1000.0, 20.0, 200.0);
        configScrolling(-1000.0, 1000.0, 0.0, -1000.0, 1000.0, 0.0);
    }
protected:

    Color getColor(double x, double y)
    {
        return waterGetPreviewCoverage(_renderer, x, y, scaling, _highlight_enabled ? 1 : 0);
    }

    void updateData()
    {
        WaterRendererClass.bind(_renderer, _definition);
        TerrainRendererClass.bind(_renderer, Scenery::getCurrent()->getTerrain());
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
    bool _highlight_enabled;
};

class PreviewWaterColor : public BasePreview
{
public:

    PreviewWaterColor(QWidget* parent) : BasePreview(parent)
    {
        _background = 0;
        _lighting_enabled = false;

        _renderer = rendererCreate();
        _renderer->atmosphere->getLightingStatus = _getLightingStatus;
        _renderer->rayWalking = _rayWalking;
        _renderer->customData[0] = this;
        //cameraSetTarget(&_renderer->render_camera, 0.0, 0.0, 0.0);

        configScaling(10.0, 1000.0, 10.0, 250.0);
        //configScrolling(-30.0, 30.0, 0.0, -20.0, 20.0, 0.0);

        addChoice("bg", tr("Background"), QStringList(tr("None")) << tr("Grid") << tr("Sinusoid"), 2);
        addToggle("light", tr("Light reflection"), true);
    }
    int _background;
    bool _lighting_enabled;
protected:

    Color getColor(double x, double y)
    {
        Vector3 eye, look;
        double target_x, target_z;

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

        target_x = eye.x - look.x * eye.y / look.y;
        target_z = eye.z - look.z * eye.y / look.y;

        if (target_z > 0.0)
        {
            return _rayWalking(_renderer, eye, look, 0, 0, 0, 0).hit_color;
        }

        return _renderer->water->getResult(_renderer, target_x, target_z).final;
    }

    void cameraEvent()
    {
        Vector3 camera_location = {0.0, scaling, -10.0 * scaling};
        cameraSetLocation(_renderer->render_camera, camera_location);
    }

    static double _getWaterHeight(Renderer*)
    {
        return 0.0;
    }

    void updateData()
    {
        WaterRendererClass.bind(_renderer, _definition);
        _renderer->terrain->getWaterHeight = _getWaterHeight;
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

    static RayCastingResult _rayWalking(Renderer* renderer, Vector3 location, Vector3 direction, int, int, int, int)
    {
        RayCastingResult result;
        PreviewWaterColor* preview = (PreviewWaterColor*) renderer->customData[0];
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
                result.hit_color = (((int) ceil(x * 0.2) % 2 == 0) ^ ((int) ceil(y * 0.2 - 0.5) % 2 == 0)) ? COLOR_WHITE : COLOR_BLACK;
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

            if (result.hit_location.y < 0.0)
            {
                if (result.hit_location.y < -renderer->water->definition->lighting_depth)
                {
                    result.hit_color = COLOR_BLACK;
                }
                else
                {
                    double attenuation = -result.hit_location.y / renderer->water->definition->lighting_depth;
                    result.hit_color.r *= 1.0 - attenuation;
                    result.hit_color.g *= 1.0 - attenuation;
                    result.hit_color.b *= 1.0 - attenuation;
                }
            }
        }

        return result;
    }

    static void _getLightingStatus(Renderer* renderer, LightStatus* status, Vector3, int)
    {
        LightDefinition light;
        PreviewWaterColor* preview = (PreviewWaterColor*) renderer->customData[0];
        light.color = COLOR_WHITE;
        light.direction.x = 0.0;
        light.direction.y = -0.4794;
        light.direction.z = -0.8776;
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
FormWater::FormWater(QWidget *parent) :
BaseForm(parent)
{
    addAutoPreset(tr("Lake surface"));
    addAutoPreset(tr("Standard sea"));

    _definition = new WaterDefinition(NULL);

    previewCoverage = new PreviewWaterCoverage(this);
    previewColor = new PreviewWaterColor(this);
    addPreview(previewCoverage, tr("Coverage preview"));
    addPreview(previewColor, tr("Rendered preview"));

    //addInputDouble(tr("Height"), &_definition->height, -15.0, 15.0, 0.1, 1.0);
    addInputMaterial(tr("Surface material"), _definition->material);
    addInputColor(tr("Depth color"), _definition->depth_color);
    addInputDouble(tr("Transparency"), &_definition->transparency, 0.0, 1.0, 0.001, 0.1);
    addInputDouble(tr("Reflection"), &_definition->reflection, 0.0, 1.0, 0.001, 0.1);
    addInputDouble(tr("Transparency distance"), &_definition->transparency_depth, 0.0, 20.0, 0.1, 1.0);
    addInputDouble(tr("Light-through distance"), &_definition->lighting_depth, 0.0, 20.0, 0.1, 1.0);
    addInputDouble(tr("Waves scaling"), &_definition->scaling, 0.02, 2.0, 0.02, 0.2);
    addInputDouble(tr("Waves height"), &_definition->waves_height, 0.0, 2.0, 0.02, 0.2);
    addInputDouble(tr("Waves detail"), &_definition->detail_height, 0.0, 0.3, 0.003, 0.03);
    addInputDouble(tr("Waves turbulence"), &_definition->turbulence, 0.0, 0.5, 0.005, 0.05);
    addInputDouble(tr("Foam coverage"), &_definition->foam_coverage, 0.0, 1.0, 0.01, 0.1);
    addInputMaterial(tr("Foam material"), _definition->foam_material);

    revertConfig();
}

void FormWater::revertConfig()
{
    Scenery::getCurrent()->getWater(_definition);
    BaseForm::revertConfig();
}

void FormWater::applyConfig()
{
    Scenery::getCurrent()->setWater(_definition);
    BaseForm::applyConfig();
}

void FormWater::configChangeEvent()
{
    _definition->validate();
    BaseForm::configChangeEvent();
}

void FormWater::autoPresetSelected(int preset)
{
    _definition->applyPreset((WaterPreset)preset);
    BaseForm::autoPresetSelected(preset);
}

