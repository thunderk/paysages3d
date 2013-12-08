#include "SoftwareRenderer.h"

#include "CameraDefinition.h"
#include "Scenery.h"
#include "FluidMediumManager.h"
#include "AtmosphereRenderer.h"
#include "AtmosphereDefinition.h"
#include "AtmosphereResult.h"
#include "CloudsRenderer.h"
#include "TerrainRenderer.h"
#include "TexturesRenderer.h"
#include "WaterRenderer.h"
#include "SkyRasterizer.h"
#include "TerrainRasterizer.h"
#include "WaterRasterizer.h"
#include "LightStatus.h"
#include "LightingManager.h"


// Legacy compatibility
#include "renderer.h"
static double _getPrecision(Renderer* renderer, Vector3 location)
{
    Vector3 projected;

    projected = renderer->render_camera->project(location);
    projected.x += 1.0;
    //projected.y += 1.0;

    return v3Norm(v3Sub(renderer->render_camera->unproject(projected), location)); // / (double)render_quality;
}

SoftwareRenderer::SoftwareRenderer(Scenery* scenery)
{
    atmosphere_renderer = new BaseAtmosphereRenderer(this);
    clouds_renderer = new CloudsRenderer(this);
    terrain_renderer = new TerrainRenderer(this);
    textures_renderer = new TexturesRenderer(this);
    water_renderer = new WaterRenderer(this);

    fluid_medium = new FluidMediumManager(this);

    if (scenery)
    {
        this->scenery = scenery;
        own_scenery = false;
    }
    else
    {
        this->scenery = new Scenery;
        own_scenery = true;
    }
}

SoftwareRenderer::~SoftwareRenderer()
{
    delete atmosphere_renderer;
    delete clouds_renderer;
    delete terrain_renderer;
    delete textures_renderer;
    delete water_renderer;

    delete fluid_medium;

    if (own_scenery)
    {
        delete scenery;
    }
}

void SoftwareRenderer::setScenery(Scenery* scenery)
{
    if (!own_scenery)
    {
        this->scenery = new Scenery;
        own_scenery = true;
    }
    scenery->copy(this->scenery);
}

void SoftwareRenderer::prepare()
{
    // Prepare sub renderers
    delete atmosphere_renderer;
    atmosphere_renderer = new SoftwareBrunetonAtmosphereRenderer(this);

    delete clouds_renderer;
    clouds_renderer = new CloudsRenderer(this);
    clouds_renderer->update();

    delete terrain_renderer;
    terrain_renderer = new TerrainRenderer(this);

    delete textures_renderer;
    textures_renderer = new TexturesRenderer(this);

    delete water_renderer;
    water_renderer = new WaterRenderer(this);

    // Setup transitional renderers (for C-legacy subsystems)
    getPrecision = _getPrecision;

    // Prepare global tools
    fluid_medium->clearMedia();
    //fluid_medium->registerMedium(water_renderer);
}

void SoftwareRenderer::rasterize()
{
    TerrainRasterizer terrain(this);
    terrain.renderSurface();

    WaterRasterizer water(this);
    water.renderSurface();

    SkyRasterizer sky(this);
    sky.rasterize();
}

Color SoftwareRenderer::applyLightingToSurface(const Vector3 &location, const Vector3 &normal, const SurfaceMaterial &material)
{
    LightStatus status(lighting, location, getCameraLocation(this, location));
    atmosphere_renderer->getLightingStatus(&status, normal, 0);
    return status.apply(normal, material);
}

Color SoftwareRenderer::applyMediumTraversal(Vector3 location, Color color)
{
    color = atmosphere_renderer->applyAerialPerspective(location, color).final;
    color = clouds_renderer->getColor(getCameraLocation(this, location), location, color);
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
        result.hit_location = v3Add(location, v3Scale(direction, 1000.0));
        result.hit_color = clouds_renderer->getColor(location, result.hit_location, sky_color);
    }

    return result;
}
