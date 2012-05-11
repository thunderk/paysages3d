#include "formclouds.h"

#include "../lib_paysages/clouds.h"
#include "../lib_paysages/color.h"
#include "../lib_paysages/euclid.h"
#include "../lib_paysages/scenery.h"
#include "../lib_paysages/shared/constants.h"

#include "tools.h"

static CloudsDefinition _definition;
static CloudsLayerDefinition _layer;

/**************** Previews ****************/
class PreviewCloudsCoverage:public BasePreview
{
public:
    PreviewCloudsCoverage(QWidget* parent):BasePreview(parent)
    {
        _renderer = rendererCreate();
        _renderer.render_quality = 3;
        
        _preview_layer = cloudsLayerCreateDefinition();
        
        configScaling(100.0, 1000.0, 20.0, 200.0);
    }
protected:
    QColor getColor(double x, double y)
    {
        Vector3 eye, look;
        Color color_layer, result;

        eye.x = 0.0;
        eye.y = scaling;
        eye.z = -10.0 * scaling;
        look.x = x * 0.01 / scaling;
        look.y = -y * 0.01 / scaling - 0.3;
        look.z = 1.0;
        look = v3Normalize(look);

        result = COLOR_BLUE;
        color_layer = cloudsGetLayerColor(&_preview_layer, &_renderer, eye, v3Add(eye, v3Scale(look, 1000.0)));
        colorMask(&result, &color_layer);
        return colorToQColor(result);
    }
    void updateData()
    {
        cloudsLayerCopyDefinition(&_layer, &_preview_layer);
    }

private:
    Renderer _renderer;
    CloudsLayerDefinition _preview_layer;
};

class PreviewCloudsColor:public BasePreview
{
public:
    PreviewCloudsColor(QWidget* parent):BasePreview(parent)
    {
        LightDefinition light;
        
        _preview_layer = cloudsLayerCreateDefinition();

        _lighting = lightingCreateDefinition();
        light.color = COLOR_WHITE;
        light.amplitude = 0.0;
        light.direction.x = -1.0;
        light.direction.y = -1.0;
        light.direction.z = 1.0;
        light.direction = v3Normalize(light.direction);
        light.filtered = 0;
        light.masked = 1;
        light.reflection = 1.0;
        lightingAddLight(&_lighting, light);
        lightingValidateDefinition(&_lighting);
        
        _renderer = rendererCreate();
        _renderer.render_quality = 3;
        _renderer.applyLightingToSurface = _applyLightingToSurface;
        _renderer.maskLight = _maskLight;
        _renderer.customData[0] = &_preview_layer;
        _renderer.customData[1] = &_lighting;
        
        configScaling(1.0, 4.0, 0.2, 2.0);
    }
protected:
    QColor getColor(double x, double y)
    {
        Vector3 start, end;
        Color color_layer, result;

        start.x = x * _preview_layer.ymax;
        start.y = -y * _preview_layer.ymax;
        start.z = _preview_layer.ymax;
        
        end.x = x * _preview_layer.ymax;
        end.y = -y * _preview_layer.ymax;
        end.z = -_preview_layer.ymax;

        result = COLOR_BLUE;
        color_layer = cloudsGetLayerColor(&_preview_layer, &_renderer, start, end);
        colorMask(&result, &color_layer);
        return colorToQColor(result);
    }
    void updateData()
    {
        cloudsLayerCopyDefinition(&_layer, &_preview_layer);
        _preview_layer.ymax = (_preview_layer.ymax - _preview_layer.ymin) / 2.0;
        _preview_layer.ycenter = 0.0;
        _preview_layer.ymin = -_preview_layer.ymin;
        _preview_layer.customcoverage = _coverageFunc;
    }
private:
    Renderer _renderer;
    CloudsLayerDefinition _preview_layer;
    LightingDefinition _lighting;

    static double _coverageFunc(CloudsLayerDefinition* layer, Vector3 position)
    {
        double dist = v3Norm(position);
        
        if (dist >= layer->ymax)
        {
            return 0.0;
        }
        else
        {
            return 1.0 - dist / layer->ymax;
        }
    }
    static Color _applyLightingToSurface(Renderer* renderer, Vector3 location, Vector3 normal, SurfaceMaterial material)
    {
        return lightingApplyToSurface((LightingDefinition*)renderer->customData[1], renderer, location, normal, material);
    }
    static Color _maskLight(Renderer* renderer, Color light_color, Vector3 at_location, Vector3 light_location, Vector3 direction_to_light)
    {
        return cloudsLayerFilterLight((CloudsLayerDefinition*)renderer->customData[0], renderer, light_color, at_location, light_location, direction_to_light);
    }
};

/**************** Form ****************/
FormClouds::FormClouds(QWidget *parent):
    BaseForm(parent, false, true)
{
    _definition = cloudsCreateDefinition();
    _layer = cloudsLayerCreateDefinition();
    
    addPreview(new PreviewCloudsCoverage(parent), tr("Layer coverage (no lighting)"));
    addPreview(new PreviewCloudsColor(parent), tr("Color and lighting"));

    addInputDouble(tr("Start altitude"), &_layer.ymin, -10.0, 250.0, 0.5, 5.0);
    addInputDouble(tr("Max density altitude"), &_layer.ycenter, -10.0, 250.0, 0.5, 5.0);
    addInputDouble(tr("End altitude"), &_layer.ymax, -10.0, 250.0, 0.5, 5.0);
    addInputNoise(tr("Noise"), _layer.noise);
    addInputDouble(tr("Coverage"), &_layer.coverage, 0.0, 1.0, 0.01, 0.1);
    addInputDouble(tr("Scaling"), &_layer.scaling, 1.0, 100.0, 0.5, 5.0);
    addInputColor(tr("Base color"), &_layer.material.base);
    addInputDouble(tr("Light reflection"), &_layer.material.reflection, 0.0, 1.0, 0.01, 0.1);
    addInputDouble(tr("Light reflection shininess"), &_layer.material.shininess, 0.0, 20.0, 0.1, 1.0);
    addInputDouble(tr("Hardness to light"), &_layer.hardness, 0.0, 1.0, 0.01, 0.1);
    addInputDouble(tr("Transparency depth"), &_layer.transparencydepth, 0.0, 100.0, 0.5, 5.0);
    addInputDouble(tr("Light traversal depth"), &_layer.lighttraversal, 0.0, 100.0, 0.5, 5.0);
    addInputDouble(tr("Minimum lighting"), &_layer.minimumlight, 0.0, 1.0, 0.01, 0.1);

    revertConfig();
}

void FormClouds::revertConfig()
{
    sceneryGetClouds(&_definition);
    setLayerCount(cloudsGetLayerCount(&_definition));
    BaseForm::revertConfig();
}

void FormClouds::applyConfig()
{
    scenerySetClouds(&_definition);
    BaseForm::applyConfig();
}

void FormClouds::configChangeEvent()
{
    cloudsLayerCopyDefinition(&_layer, cloudsGetLayer(&_definition, currentLayer()));
    cloudsValidateDefinition(&_definition);
    BaseForm::configChangeEvent();
}

void FormClouds::layerAddedEvent()
{
    if (cloudsAddLayer(&_definition) >= 0)
    {
        BaseForm::layerAddedEvent();
    }
}

void FormClouds::layerDeletedEvent(int layer)
{
    cloudsDeleteLayer(&_definition, layer);
    
    BaseForm::layerDeletedEvent(layer);
}

void FormClouds::layerSelectedEvent(int layer)
{
    cloudsLayerCopyDefinition(cloudsGetLayer(&_definition, layer), &_layer);
    
    BaseForm::layerSelectedEvent(layer);
}
