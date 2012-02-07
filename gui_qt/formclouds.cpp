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
class PreviewCloudsCoverage:public Preview
{
public:
    PreviewCloudsCoverage(QWidget* parent):Preview(parent)
    {
        _renderer = rendererCreate();
        _renderer.render_quality = 3;
        
        _preview_layer = cloudsLayerCreateDefinition();
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

/**************** Form ****************/
FormClouds::FormClouds(QWidget *parent):
    BaseForm(parent, false, true)
{
    _definition = cloudsCreateDefinition();
    _layer = cloudsLayerCreateDefinition();
    
    addPreview(new PreviewCloudsCoverage(parent), "Layer coverage (no lighting)");

    addInputDouble("Start altitude", &_layer.ymin, -10.0, 250.0, 0.5, 5.0);
    addInputDouble("Max density altitude", &_layer.ycenter, -10.0, 250.0, 0.5, 5.0);
    addInputDouble("End altitude", &_layer.ymax, -10.0, 250.0, 0.5, 5.0);
    addInputNoise("Noise", _layer.noise);
    addInputDouble("Coverage", &_layer.coverage, 0.0, 1.0, 0.01, 0.1);
    addInputDouble("Scaling", &_layer.scaling, 1.0, 100.0, 0.5, 5.0);
    addInputColor("Base color", &_layer.material.base);
    addInputDouble("Light reflection", &_layer.material.reflection, 0.0, 1.0, 0.01, 0.1);
    addInputDouble("Light reflection shininess", &_layer.material.shininess, 0.0, 20.0, 0.1, 1.0);
    addInputDouble("Transparency depth", &_layer.transparencydepth, 0.0, 100.0, 0.5, 5.0);
    addInputDouble("Light traversal depth", &_layer.lighttraversal, 0.0, 100.0, 0.5, 5.0);
    addInputDouble("Minimum lighting", &_layer.minimumlight, 0.0, 1.0, 0.01, 0.1);

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
    if (cloudsAddLayer(&_definition) > 0)
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
