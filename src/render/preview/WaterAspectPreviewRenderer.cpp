#include "WaterAspectPreviewRenderer.h"

#include "BasePreview.h"
#include "Scenery.h"
#include "WaterDefinition.h"
#include "CameraDefinition.h"
#include "WaterRenderer.h"
#include "LightComponent.h"

WaterAspectPreviewRenderer::WaterAspectPreviewRenderer(WaterDefinition* definition):
    definition(definition)
{
    lighting = true;
    background = 2;

    render_quality = 3;

    customData[0] = &lighting;
    customData[1] = &background;
}

void WaterAspectPreviewRenderer::bindEvent(BasePreview* preview)
{
    preview->configScaling(10.0, 1000.0, 10.0, 250.0);
    //configScrolling(-30.0, 30.0, 0.0, -20.0, 20.0, 0.0);

    // TODO Translation
    preview->addChoice("bg", "Background", QStringList("None") << "Grid" << "Sinusoid", 2);
    preview->addToggle("light", "Light reflection", true);

    //preview->configHdrToneMapping(true);
}

void WaterAspectPreviewRenderer::updateEvent()
{
    getScenery()->setWater(definition);
    getScenery()->getCamera()->setTarget(VECTOR_ZERO);
    prepare();

    LightComponent light;
    std::vector<LightComponent> lights;
    light.color = COLOR_WHITE;
    light.direction.x = 0.0;
    light.direction.y = -0.4794;
    light.direction.z = -0.8776;
    light.altered = 0;
    if (lighting)
    {
        light.reflection = 1.0;
    }
    else
    {
        light.reflection = 0.0;
    }
    lights.push_back(light);
    disableAtmosphere(lights);
}

void WaterAspectPreviewRenderer::cameraEvent(double, double, double scaling)
{
    Vector3 camera_location(0.0, scaling, -10.0 * scaling);
    getScenery()->getCamera()->setLocation(camera_location);
    render_camera->setLocation(camera_location);
}

Color WaterAspectPreviewRenderer::getColor2D(double x, double y, double scaling)
{
    Vector3 eye, look;
    double target_x, target_z;

    eye = render_camera->getLocation();
    look = Vector3(x * 0.01 / scaling, -y * 0.01 / scaling - 0.3, 1.0).normalize();

    if (look.y > -0.0001)
    {
        return rayWalking(eye, look, 0, 0, 0, 0).hit_color;
    }

    target_x = eye.x - look.x * eye.y / look.y;
    target_z = eye.z - look.z * eye.y / look.y;

    if (target_z > 0.0)
    {
        return rayWalking(eye, look, 0, 0, 0, 0).hit_color;
    }

    return getWaterRenderer()->getResult(target_x, target_z).final;
}

void WaterAspectPreviewRenderer::toggleChangeEvent(const std::string &key, bool value)
{
    if (key == "light")
    {
        lighting = value;
    }
}

void WaterAspectPreviewRenderer::choiceChangeEvent(const std::string &key, int position)
{
    if (key == "bg")
    {
        background = position;
    }
}

double WaterAspectPreviewRenderer::getPrecision(const Vector3 &)
{
    return 0.000001;
}

RayCastingResult WaterAspectPreviewRenderer::rayWalking(const Vector3 &location, const Vector3 &direction, int, int, int, int)
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

        switch (background)
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
            double lighting_depth = getScenery()->getWater()->lighting_depth;
            if (result.hit_location.y < -lighting_depth)
            {
                result.hit_color = COLOR_BLACK;
            }
            else
            {
                double attenuation = -result.hit_location.y / lighting_depth;
                result.hit_color.r *= 1.0 - attenuation;
                result.hit_color.g *= 1.0 - attenuation;
                result.hit_color.b *= 1.0 - attenuation;
            }
        }
    }

    return result;
}
