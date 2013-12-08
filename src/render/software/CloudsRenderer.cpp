#include "CloudsRenderer.h"

#include "SoftwareRenderer.h"
#include "Scenery.h"
#include "CloudsDefinition.h"
#include "CloudLayerDefinition.h"
#include "BaseCloudLayerRenderer.h"
#include "CloudBasicLayerRenderer.h"

#include "clouds/BaseCloudsModel.h"
#include "clouds/CloudModelStratoCumulus.h"

CloudsRenderer::CloudsRenderer(SoftwareRenderer* parent):
    parent(parent)
{
    fake_renderer = new BaseCloudLayerRenderer(parent);

    CloudLayerDefinition* fake_layer = new CloudLayerDefinition(NULL);
    fake_model = new BaseCloudsModel(fake_layer);
}

CloudsRenderer::~CloudsRenderer()
{
    for (auto renderer : layer_renderers)
    {
        delete renderer;
    }
    delete fake_renderer;

    for (auto model : layer_models)
    {
        delete model;
    }
    delete fake_model->getLayer();
    delete fake_model;
}

void CloudsRenderer::update()
{
    for (auto renderer : layer_renderers)
    {
        delete renderer;
    }
    layer_renderers.clear();

    for (auto model : layer_models)
    {
        delete model;
    }
    layer_models.clear();

    CloudsDefinition* clouds = parent->getScenery()->getClouds();
    int n = clouds->count();
    for (int i = 0; i < n; i++)
    {
        layer_renderers.push_back(new CloudBasicLayerRenderer(parent));

        CloudLayerDefinition* layer = clouds->getCloudLayer(i);
        BaseCloudsModel* model;
        switch (layer->type)
        {
        case CloudLayerDefinition::STRATUS:
        case CloudLayerDefinition::NIMBOSTRATUS:
        case CloudLayerDefinition::CUMULUS:
            model = new BaseCloudsModel(layer);
            break;
        case CloudLayerDefinition::STRATOCUMULUS:
            model = new CloudModelStratoCumulus(layer);
            break;
        case CloudLayerDefinition::ALTOCUMULUS:
        case CloudLayerDefinition::ALTOSTRATUS:
        case CloudLayerDefinition::CUMULONIMBUS:
        case CloudLayerDefinition::CIRROCUMULUS:
        case CloudLayerDefinition::CIRROSTRATUS:
        case CloudLayerDefinition::CIRRUS:
            model = new BaseCloudsModel(layer);
            break;
        }

        layer_models.push_back(model);
        model->update();
    }
}

BaseCloudLayerRenderer* CloudsRenderer::getLayerRenderer(unsigned int layer)
{
    if (layer < layer_renderers.size())
    {
        return layer_renderers[layer];
    }
    else
    {
        return fake_renderer;
    }
}

BaseCloudsModel* CloudsRenderer::getLayerModel(unsigned int layer)
{
    if (layer < layer_models.size())
    {
        return layer_models[layer];
    }
    else
    {
        return fake_model;
    }
}

Color CloudsRenderer::getColor(const Vector3 &eye, const Vector3 &location, const Color &base)
{
    CloudsDefinition* definition = parent->getScenery()->getClouds();

    int n = definition->count();
    if (n < 1)
    {
        return base;
    }

    /* TODO Iter layers in sorted order */
    Color cumul = base;

    for (int i = 0; i < n; i++)
    {
        BaseCloudLayerRenderer* layer_renderer = getLayerRenderer(i);
        BaseCloudsModel* layer_model = getLayerModel(i);

        Color layer_color = layer_renderer->getColor(layer_model, eye, location);

        colorMask(&cumul, &layer_color);
    }

    return cumul;
}

bool CloudsRenderer::alterLight(LightComponent* light, const Vector3 &eye, const Vector3 &location)
{
    CloudsDefinition* definition = parent->getScenery()->getClouds();

    int n = definition->count();
    if (n < 1)
    {
        return false;
    }

    /* TODO Iter layers in sorted order */
    bool altered = false;
    for (int i = 0; i < n; i++)
    {
        BaseCloudLayerRenderer* layer_renderer = getLayerRenderer(i);
        BaseCloudsModel* layer_model = getLayerModel(i);

        altered = altered || layer_renderer->alterLight(layer_model, light, eye, location);
    }

    return altered;
}
