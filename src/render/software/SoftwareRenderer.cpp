#include "SoftwareRenderer.h"

#include "CameraDefinition.h"
#include "Scenery.h"
#include "FluidMediumManager.h"
#include "AtmosphereRenderer.h"
#include "AtmosphereDefinition.h"
#include "AtmosphereResult.h"
#include "CloudsRenderer.h"
#include "CloudsDefinition.h"
#include "TerrainRenderer.h"
#include "TexturesRenderer.h"
#include "WaterRenderer.h"
#include "SkyRasterizer.h"
#include "TerrainRasterizer.h"
#include "WaterRasterizer.h"
#include "NightSky.h"
#include "LightStatus.h"
#include "LightingManager.h"
#include "System.h"
#include "Thread.h"

SoftwareRenderer::SoftwareRenderer(Scenery* scenery):
    scenery(scenery)
{
    render_quality = 5;
    render_camera = new CameraDefinition;

    scenery->getCamera()->copy(render_camera);

    atmosphere_renderer = new BaseAtmosphereRenderer(this);
    clouds_renderer = new CloudsRenderer(this);
    terrain_renderer = new TerrainRenderer(this);
    textures_renderer = new TexturesRenderer(this);
    water_renderer = new WaterRenderer(this);

    nightsky_renderer = new NightSky(this);

    fluid_medium = new FluidMediumManager(this);
    lighting = new LightingManager();

    lighting->registerFilter(water_renderer);
    lighting->registerFilter(terrain_renderer);
    lighting->registerFilter(clouds_renderer);
}

SoftwareRenderer::~SoftwareRenderer()
{
    delete render_camera;

    delete fluid_medium;
    delete lighting;

    delete nightsky_renderer;

    delete atmosphere_renderer;
    delete clouds_renderer;
    delete terrain_renderer;
    delete textures_renderer;
    delete water_renderer;
}

void SoftwareRenderer::prepare()
{
    // Prepare sub renderers
    // TODO Don't recreate the renderer each time
    delete atmosphere_renderer;
    if (getScenery()->getAtmosphere()->model == AtmosphereDefinition::ATMOSPHERE_MODEL_BRUNETON)
    {
        atmosphere_renderer = new SoftwareBrunetonAtmosphereRenderer(this);
    }
    else
    {
        atmosphere_renderer = new BaseAtmosphereRenderer(this);
    }

    clouds_renderer->update();
    terrain_renderer->update();
    water_renderer->update();
    textures_renderer->update();

    nightsky_renderer->update();

    // Prepare global tools
    fluid_medium->clearMedia();
    //fluid_medium->registerMedium(water_renderer);
}

void SoftwareRenderer::disableAtmosphere()
{
    LightComponent light;
    std::vector<LightComponent> lights;

    light.color.r = 1.5;
    light.color.g = 1.5;
    light.color.b = 1.5;
    light.direction.x = -1.0;
    light.direction.y = -0.3;
    light.direction.z = 1.0;
    light.direction = light.direction.normalize();
    light.altered = 1;
    light.reflection = 0.0;
    lights.push_back(light);

    light.color.r = 0.25;
    light.color.g = 0.25;
    light.color.b = 0.265;
    light.direction.x = 1.0;
    light.direction.y = -0.5;
    light.direction.z = -1.0;
    light.direction = light.direction.normalize();
    light.altered = 0;
    light.reflection = 0.0;
    lights.push_back(light);

    disableAtmosphere(lights);
}

void SoftwareRenderer::disableAtmosphere(const std::vector<LightComponent> &lights)
{
    scenery->getAtmosphere()->model = AtmosphereDefinition::ATMOSPHERE_MODEL_DISABLED;

    delete atmosphere_renderer;
    atmosphere_renderer = new BaseAtmosphereRenderer(this);
    atmosphere_renderer->setStaticLights(lights);
}

Color SoftwareRenderer::applyLightingToSurface(const Vector3 &location, const Vector3 &normal, const SurfaceMaterial &material)
{
    LightStatus status(lighting, location, getCameraLocation(location));
    atmosphere_renderer->getLightingStatus(&status, normal, 0);
    return status.apply(normal, material);
}

Color SoftwareRenderer::applyMediumTraversal(Vector3 location, Color color)
{
    color = atmosphere_renderer->applyAerialPerspective(location, color).final;
    color = clouds_renderer->getColor(getCameraLocation(location), location, color);
    return color;

    /*Vector3 eye = cameraGetLocation(scenery->getCamera());
    return fluid_medium->applyTraversal(eye, location, color);*/
}

RayCastingResult SoftwareRenderer::rayWalking(const Vector3 &location, const Vector3 &direction, int, int, int, int)
{
    RayCastingResult result;
    Color sky_color;

    result = terrain_renderer->castRay(location, direction);
    if (!result.hit)
    {
        sky_color = atmosphere_renderer->getSkyColor(direction).final;

        result.hit = 1;
        result.hit_location = location.add(direction.scale(1000.0));
        result.hit_color = clouds_renderer->getColor(location, result.hit_location, sky_color);
    }

    return result;
}

Vector3 SoftwareRenderer::getCameraLocation(const Vector3 &)
{
    return render_camera->getLocation();
}

Vector3 SoftwareRenderer::getCameraDirection(const Vector3 &)
{
    return render_camera->getDirectionNormalized();
}

double SoftwareRenderer::getPrecision(const Vector3 &location)
{
    Vector3 projected;

    projected = render_camera->project(location);
    projected.x += 1.0;
    //projected.y += 1.0;

    return render_camera->unproject(projected).sub(location).getNorm(); // / (double)render_quality;
}

Vector3 SoftwareRenderer::projectPoint(const Vector3 &point)
{
    return render_camera->project(point);
}

Vector3 SoftwareRenderer::unprojectPoint(const Vector3 &point)
{
    return render_camera->unproject(point);
}
