#include "CloudsAspectPreviewRenderer.h"

#include "BasePreview.h"
#include "Scenery.h"
#include "clouds/BaseCloudsModel.h"
#include "CloudsDefinition.h"
#include "CloudLayerDefinition.h"
#include "CloudsRenderer.h"
#include "LightStatus.h"
#include "LightComponent.h"
#include "AtmosphereResult.h"

class FakeCloudModel:public BaseCloudsModel
{
public:
    FakeCloudModel(BaseCloudsModel* real_model):
        BaseCloudsModel(real_model->getLayer()), real_model(real_model)
    {
    }

    virtual ~FakeCloudModel()
    {
        delete real_model;
    }

    virtual void update()
    {
        real_model->update();
    }

    virtual void getAltitudeRange(double *min_altitude, double *max_altitude) const
    {
        real_model->getAltitudeRange(min_altitude, max_altitude);
    }

    virtual void getDetailRange(double *min_step, double *max_step) const
    {
        real_model->getDetailRange(min_step, max_step);
        *min_step *= 0.4;
        *max_step *= 0.4;
    }

    virtual double getProbability(const Vector3 &location, double radius) const
    {
        return real_model->getProbability(location, radius);
    }

    virtual Color filterLight(const Color &light, double length, double density) const
    {
        return real_model->filterLight(light, length, density);
    }

    virtual Color applyLightExit(const Color &light, const Vector3 &light_direction, const Vector3 &direction_to_eye) const
    {
        return real_model->applyLightExit(light, light_direction, direction_to_eye);
    }

    virtual double getDensity(const Vector3 &location) const override
    {
        double ymin, ymax, thickness;
        getAltitudeRange(&ymin, &ymax);
        thickness = ymax - ymin;

        Vector3 center(0.0, ymin + thickness * 0.5, 0.0);
        double distance = 2.0 * location.sub(center).getNorm() / thickness;
        double fallout = 0.7;
        if (distance > 1.0)
        {
            return 0.0;
        }
        else if (distance < 1.0 - fallout)
        {
            return 1.0;
        }
        else
        {
            double factor = (1.0 - distance) / fallout;
            return real_model->getDensity(location) * (1.0 - factor) + factor;
        }
    }

private:
    BaseCloudsModel* real_model;
};

CloudsAspectPreviewRenderer::CloudsAspectPreviewRenderer(CloudLayerDefinition* layer):
    layer(layer)
{
    CloudsDefinition* clouds = getScenery()->getClouds();
    clouds->clear();
    clouds->addLayer();

    render_quality = 6;
}

void CloudsAspectPreviewRenderer::bindEvent(BasePreview* preview)
{
    preview->configScaling(0.5, 2.0, 0.1, 2.0);
    preview->configHdrToneMapping(true);
}

void CloudsAspectPreviewRenderer::updateEvent()
{
    CloudLayerDefinition* preview_layer = getScenery()->getClouds()->getCloudLayer(0);
    layer->copy(preview_layer);
    preview_layer->coverage = 0.5;

    prepare();

    disableAtmosphere();
    BaseCloudsModel* real_model = getCloudsRenderer()->getLayerModel(0);
    getCloudsRenderer()->setLayerModel(0, new FakeCloudModel(real_model), false);
}

Color CloudsAspectPreviewRenderer::getColor2D(double x, double y, double)
{
    Vector3 start, end;
    BaseCloudsModel* model = getCloudsRenderer()->getLayerModel(0);
    double ymin, ymax, thickness;
    model->getAltitudeRange(&ymin, &ymax);
    thickness = ymax - ymin;

    start.x = x * thickness * 0.5;
    start.y = ymin + (1.0 - y) * thickness * 0.5;
    start.z = thickness * 0.5;

    end.x = start.x;
    end.y = start.y;
    end.z = -start.z;

    return getCloudsRenderer()->getColor(start, end, COLOR_BLUE);
}
