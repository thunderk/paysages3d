#include "SoftwareRenderer.h"

#include "AtmosphereDefinition.h"
#include "AtmosphereRenderer.h"
#include "AtmosphereResult.h"
#include "CameraDefinition.h"
#include "CloudsDefinition.h"
#include "CloudsRenderer.h"
#include "FluidMediumManager.h"
#include "GodRaysResult.h"
#include "GodRaysSampler.h"
#include "LightStatus.h"
#include "LightingManager.h"
#include "MoonRenderer.h"
#include "NightSky.h"
#include "RayCastingResult.h"
#include "Scenery.h"
#include "SkyRasterizer.h"
#include "System.h"
#include "TerrainRasterizer.h"
#include "TerrainRenderer.h"
#include "TexturesRenderer.h"
#include "Thread.h"
#include "VegetationRenderer.h"
#include "WaterRasterizer.h"
#include "WaterRenderer.h"

SoftwareRenderer::SoftwareRenderer(Scenery *scenery) : scenery(scenery) {
    render_camera = new CameraDefinition;

    scenery->getCamera()->copy(render_camera);

    atmosphere_renderer = new BaseAtmosphereRenderer(this);
    clouds_renderer = new CloudsRenderer(this);
    terrain_renderer = new TerrainRenderer(this);
    textures_renderer = new TexturesRenderer();
    vegetation_renderer = new VegetationRenderer(this);
    water_renderer = new WaterRenderer(this);

    nightsky_renderer = new NightSky(this);
    moon_renderer = new MoonRenderer(scenery->getAtmosphere()->childMoon());

    fluid_medium = new FluidMediumManager(this); // TODO Not used yet
    lighting = new LightingManager();
    godrays = new GodRaysSampler();

    aerial_perspective = true;

    lighting->registerFilter(water_renderer);
    lighting->registerFilter(terrain_renderer);
    lighting->registerFilter(vegetation_renderer);
    lighting->registerFilter(clouds_renderer);
    lighting->registerSource(atmosphere_renderer);
    lighting->registerSource(moon_renderer);

    setQuality(0.5);
}

SoftwareRenderer::~SoftwareRenderer() {
    moon_renderer->unregister();

    delete render_camera;

    delete fluid_medium;
    delete lighting;
    delete godrays;

    delete nightsky_renderer;
    delete moon_renderer;

    delete atmosphere_renderer;
    delete clouds_renderer;
    delete terrain_renderer;
    delete textures_renderer;
    delete vegetation_renderer;
    delete water_renderer;
}

void SoftwareRenderer::prepare() {
    scenery->validate();
    scenery->getCamera()->copy(render_camera);

    // Prepare sub renderers
    // TODO Don't recreate the renderer each time, only when it changes
    lighting->unregisterSource(atmosphere_renderer);
    delete atmosphere_renderer;
    if (getScenery()->getAtmosphere()->model == AtmosphereDefinition::ATMOSPHERE_MODEL_BRUNETON) {
        atmosphere_renderer = new SoftwareBrunetonAtmosphereRenderer(this);
    } else {
        atmosphere_renderer = new BaseAtmosphereRenderer(this);
    }
    lighting->registerSource(atmosphere_renderer);

    clouds_renderer->update();
    terrain_renderer->update();
    water_renderer->update();

    nightsky_renderer->update();

    // Prepare global tools
    godrays->prepare(this);
    fluid_medium->clearMedia();
    // fluid_medium->registerMedium(water_renderer);
}

void SoftwareRenderer::setQuality(double quality) {
    terrain_renderer->setQuality(quality);
    textures_renderer->setQuality(quality);
    clouds_renderer->setQuality(quality);
    godrays->setQuality(quality);

    // TEMP compat with old code
    render_quality = trunc_to_int(quality * 9.0) + 1;
}

void SoftwareRenderer::setAerialPerspectiveEnabled(bool enabled) {
    aerial_perspective = enabled;
}

Color SoftwareRenderer::applyLightingToSurface(const Vector3 &location, const Vector3 &normal,
                                               const SurfaceMaterial &material) {
    return lighting->apply(getCameraLocation(), location, normal, material);
}

Color SoftwareRenderer::applyMediumTraversal(const Vector3 &location, const Color &color) {
    if (aerial_perspective) {
        Color result = atmosphere_renderer->applyAerialPerspective(location, color).final;
        result = clouds_renderer->getColor(getCameraLocation(), location, result);
        return result;
    } else {
        return color;
    }
}

RayCastingResult SoftwareRenderer::rayWalking(const Vector3 &location, const Vector3 &direction, int, int, int, int) {
    RayCastingResult result;
    Color sky_color;

    result = terrain_renderer->castRay(location, direction);
    if (!result.hit) {
        sky_color = atmosphere_renderer->getSkyColor(direction).final;

        result.hit = 1;
        result.hit_location = location.add(direction.scale(1000.0));
        result.hit_color = clouds_renderer->getColor(location, result.hit_location, sky_color);
    }

    return result;
}

Vector3 SoftwareRenderer::getCameraLocation() {
    return render_camera->getLocation();
}

Vector3 SoftwareRenderer::getCameraDirection() {
    return render_camera->getDirectionNormalized();
}

double SoftwareRenderer::getPrecision(const Vector3 &location) {
    Vector3 projected;

    projected = render_camera->project(location);
    projected.x += 1.0;
    // projected.y += 1.0;

    return render_camera->unproject(projected).sub(location).getNorm() * 0.1 / to_double(render_quality);
}

Vector3 SoftwareRenderer::projectPoint(const Vector3 &point) {
    return render_camera->project(point);
}

Vector3 SoftwareRenderer::unprojectPoint(const Vector3 &point) {
    return render_camera->unproject(point);
}
