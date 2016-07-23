#include "CloudsRenderer.h"

#include "BaseCloudLayerRenderer.h"
#include "CameraDefinition.h"
#include "CloudBasicLayerRenderer.h"
#include "CloudLayerDefinition.h"
#include "CloudsDefinition.h"
#include "Logs.h"
#include "Scenery.h"
#include "SoftwareRenderer.h"

#include "clouds/BaseCloudsModel.h"
#include "clouds/CloudModelAltoCumulus.h"
#include "clouds/CloudModelCirrus.h"
#include "clouds/CloudModelCumuloNimbus.h"
#include "clouds/CloudModelStratoCumulus.h"

CloudsRenderer::CloudsRenderer(SoftwareRenderer *parent) : parent(parent) {
    quality = 0.5;
    enabled = true;
    fake_renderer = new BaseCloudLayerRenderer(parent);

    CloudLayerDefinition *fake_layer = new CloudLayerDefinition(NULL, "#fake#");
    fake_model = new BaseCloudsModel(fake_layer);
}

CloudsRenderer::~CloudsRenderer() {
    for (auto renderer : layer_renderers) {
        delete renderer;
    }
    delete fake_renderer;

    for (auto model : layer_models) {
        delete model;
    }
    delete fake_model->getLayer();
    delete fake_model;
}

void CloudsRenderer::setQuality(double factor) {
    this->quality = factor;
    for (auto &renderer : layer_renderers) {
        renderer->setQuality(factor);
    }
}

void CloudsRenderer::setEnabled(bool enabled) {
    this->enabled = enabled;
}

void CloudsRenderer::update() {
    for (auto renderer : layer_renderers) {
        delete renderer;
    }
    layer_renderers.clear();

    for (auto model : layer_models) {
        delete model;
    }
    layer_models.clear();

    CloudsDefinition *clouds = parent->getScenery()->getClouds();
    int n = clouds->getLayerCount();
    for (int i = 0; i < n; i++) {
        layer_renderers.push_back(new CloudBasicLayerRenderer(parent));

        CloudLayerDefinition *layer = clouds->getCloudLayer(i);
        BaseCloudsModel *model;
        switch (layer->type) {
        case CloudLayerDefinition::STRATOCUMULUS:
            model = new CloudModelStratoCumulus(layer);
            break;
        case CloudLayerDefinition::ALTOCUMULUS:
            model = new CloudModelAltoCumulus(layer);
            break;
        case CloudLayerDefinition::CIRRUS:
            model = new CloudModelCirrus(layer);
            break;
        case CloudLayerDefinition::CUMULONIMBUS:
            model = new CloudModelCumuloNimbus(layer);
            break;
        case CloudLayerDefinition::STRATUS:
        case CloudLayerDefinition::NIMBOSTRATUS:
        case CloudLayerDefinition::CUMULUS:
        case CloudLayerDefinition::ALTOSTRATUS:
        case CloudLayerDefinition::CIRROCUMULUS:
        case CloudLayerDefinition::CIRROSTRATUS:
        case CloudLayerDefinition::_COUNT:
            model = new BaseCloudsModel(layer);
            break;
        }

        layer_models.push_back(model);
        model->update();
    }

    setQuality(quality);
}

BaseCloudLayerRenderer *CloudsRenderer::getLayerRenderer(unsigned int layer) {
    if (layer < layer_renderers.size()) {
        return layer_renderers[layer];
    } else {
        Logs::warning("Software.Clouds") << "Asked for unknown layer renderer " << layer << endl;
        return fake_renderer;
    }
}

BaseCloudsModel *CloudsRenderer::getLayerModel(unsigned int layer) {
    if (layer < layer_models.size()) {
        return layer_models[layer];
    } else {
        Logs::warning("Software.Clouds") << "Asked for unknown layer model" << layer << endl;
        return fake_model;
    }
}

void CloudsRenderer::setLayerModel(unsigned int layer, BaseCloudsModel *model, bool delete_old) {
    if (layer < layer_models.size()) {
        if (delete_old) {
            delete layer_models[layer];
        }
        layer_models[layer] = model;
    } else {
        Logs::warning("Software.Clouds") << "Asked to set an unknown layer model" << layer << endl;
        delete model;
    }
}

Color CloudsRenderer::getColor(const Vector3 &eye, const Vector3 &location, const Color &base) {
    CloudsDefinition *definition = parent->getScenery()->getClouds();

    int n = definition->getLayerCount();
    if (not enabled or n < 1) {
        return base;
    }

    /* TODO Iter layers in sorted order */
    Color cumul = base;

    for (int i = 0; i < n; i++) {
        BaseCloudLayerRenderer *layer_renderer = getLayerRenderer(i);
        BaseCloudsModel *layer_model = getLayerModel(i);

        Color layer_color = layer_renderer->getColor(layer_model, eye, location);

        cumul.mask(layer_color);
    }

    return cumul;
}

bool CloudsRenderer::applyLightFilter(LightComponent &light, const Vector3 &at) {
    CloudsDefinition *definition = parent->getScenery()->getClouds();

    int n = definition->getLayerCount();
    if (not enabled or n < 1) {
        return true;
    }

    /* TODO Iter layers in sorted order */
    for (int i = 0; i < n; i++) {
        BaseCloudLayerRenderer *layer_renderer = getLayerRenderer(i);
        BaseCloudsModel *layer_model = getLayerModel(i);

        layer_renderer->alterLight(layer_model, &light, parent->render_camera->getLocation(), at);
    }

    return true;
}

double CloudsRenderer::getHighestAltitude() {
    CloudsDefinition *definition = parent->getScenery()->getClouds();
    double highest = 0.0;

    int n = definition->getLayerCount();
    double low, high;
    for (int i = 0; i < n; i++) {
        BaseCloudsModel *layer_model = getLayerModel(i);
        layer_model->getAltitudeRange(&low, &high);
        if (high > highest) {
            highest = high;
        }
    }

    return highest;
}
