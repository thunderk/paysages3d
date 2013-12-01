#include "CloudsRenderer.h"

#include "SoftwareRenderer.h"
#include "Scenery.h"
#include "CloudsDefinition.h"
#include "BaseCloudLayerRenderer.h"
#include "CloudBasicLayerRenderer.h"

CloudsRenderer::CloudsRenderer(SoftwareRenderer* parent):
    parent(parent)
{
    fake_renderer = new BaseCloudLayerRenderer(parent);
}

CloudsRenderer::~CloudsRenderer()
{
    for (auto renderer : layer_renderers)
    {
        delete renderer;
    }
    delete fake_renderer;
}

void CloudsRenderer::update()
{
    for (auto renderer : layer_renderers)
    {
        delete renderer;
    }
    layer_renderers.clear();

    CloudsDefinition* clouds = parent->getScenery()->getClouds();
    int n = clouds->count();
    for (int i = 0; i < n; i++)
    {
        layer_renderers.push_back(new CloudBasicLayerRenderer(parent));
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
        CloudLayerDefinition* layer = definition->getCloudLayer(i);
        BaseCloudLayerRenderer* layer_renderer = getLayerRenderer(i);

        Color layer_color = layer_renderer->getColor(layer, eye, location);

        colorMask(&cumul, &layer_color);
    }

    return cumul;
}

bool CloudsRenderer::alterLight(LightDefinition* light, const Vector3 &eye, const Vector3 &location)
{
    return false;
}
