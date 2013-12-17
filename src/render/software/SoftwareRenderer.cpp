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
#include "LightStatus.h"
#include "LightingManager.h"
#include "System.h"
#include "Thread.h"

SoftwareRenderer::SoftwareRenderer(Scenery* scenery)
{
    RenderArea::RenderParams params = {1, 1, 1, 5};

    render_quality = 5;
    render_width = 1;
    render_height = 1;
    render_interrupt = 0;
    render_progress = 0.0;
    is_rendering = 0;
    render_camera = new CameraDefinition;
    render_area = new RenderArea(this);
    render_area->setParams(params);

    atmosphere_renderer = new BaseAtmosphereRenderer(this);
    clouds_renderer = new CloudsRenderer(this);
    terrain_renderer = new TerrainRenderer(this);
    textures_renderer = new TexturesRenderer(this);
    water_renderer = new WaterRenderer(this);

    fluid_medium = new FluidMediumManager(this);
    lighting = new LightingManager();

    lighting->registerFilter(terrain_renderer);
    lighting->registerFilter(water_renderer);
    lighting->registerFilter(clouds_renderer);

    this->scenery = new Scenery;
    if (scenery)
    {
        scenery->copy(this->scenery);
    }
}

SoftwareRenderer::~SoftwareRenderer()
{
    delete render_camera;
    delete render_area;

    delete fluid_medium;
    delete lighting;

    delete atmosphere_renderer;
    delete clouds_renderer;
    delete terrain_renderer;
    delete textures_renderer;
    delete water_renderer;

    delete scenery;
}

void SoftwareRenderer::setScenery(Scenery* scenery)
{
    scenery->copy(this->scenery);
}

void SoftwareRenderer::prepare()
{
    scenery->getCamera()->copy(render_camera);

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

void SoftwareRenderer::disableClouds()
{
    scenery->getClouds()->clear();
}

void SoftwareRenderer::disableAtmosphere(const std::vector<LightComponent> &lights)
{
    scenery->getAtmosphere()->model = AtmosphereDefinition::ATMOSPHERE_MODEL_DISABLED;

    delete atmosphere_renderer;
    atmosphere_renderer = new BaseAtmosphereRenderer(this);
    atmosphere_renderer->setStaticLights(lights);
}

void SoftwareRenderer::setPreviewCallbacks(RenderArea::RenderCallbackStart start, RenderArea::RenderCallbackDraw draw, RenderArea::RenderCallbackUpdate update)
{
    render_area->setPreviewCallbacks(start, draw, update);
}

static void* _renderFirstPass(void* data)
{
    SoftwareRenderer* renderer = (SoftwareRenderer*)data;
    renderer->rasterize();
    renderer->is_rendering = 0;
    return NULL;
}

void SoftwareRenderer::start(RenderArea::RenderParams params)
{
    Thread thread(_renderFirstPass);
    int loops;
    int core_count = System::getCoreCount();

    params.antialias = (params.antialias < 1) ? 1 : params.antialias;
    params.antialias = (params.antialias > 4) ? 4 : params.antialias;

    render_quality = params.quality;
    render_width = params.width * params.antialias;
    render_height = params.height * params.antialias;
    render_interrupt = 0;
    render_progress = 0.0;

    prepare();

    render_camera->setRenderSize(render_width, render_height);

    render_area->setBackgroundColor(COLOR_BLACK);
    render_area->setParams(params);
    render_area->clear();

    is_rendering = 1;
    thread.start(this);
    loops = 0;

    while (is_rendering)
    {
        Thread::timeSleepMs(100);

        if (++loops >= 10)
        {
            render_area->update();
            loops = 0;
        }
    }
    thread.join();

    is_rendering = 1;
    render_area->postProcess(core_count);
    is_rendering = 0;
}

void SoftwareRenderer::interrupt()
{
    render_interrupt = 1;
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

int SoftwareRenderer::addRenderProgress(double)
{
    return not render_interrupt;
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

void SoftwareRenderer::pushTriangle(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3, RenderArea::f_RenderFragmentCallback callback, void* callback_data)
{
    Vector3 p1, p2, p3;

    p1 = projectPoint(v1);
    p2 = projectPoint(v2);
    p3 = projectPoint(v3);

    render_area->pushTriangle(p1, p2, p3, v1, v2, v3, callback, callback_data);
}

void SoftwareRenderer::pushQuad(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3, const Vector3 &v4, RenderArea::f_RenderFragmentCallback callback, void* callback_data)
{
    pushTriangle(v2, v3, v1, callback, callback_data);
    pushTriangle(v4, v1, v3, callback, callback_data);
}

void SoftwareRenderer::pushDisplacedTriangle(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3, const Vector3 &ov1, const Vector3 &ov2, const Vector3 &ov3, RenderArea::f_RenderFragmentCallback callback, void* callback_data)
{
    Vector3 p1, p2, p3;

    p1 = projectPoint(v1);
    p2 = projectPoint(v2);
    p3 = projectPoint(v3);

    render_area->pushTriangle(p1, p2, p3, ov1, ov2, ov3, callback, callback_data);
}

void SoftwareRenderer::pushDisplacedQuad(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3, const Vector3 &v4, const Vector3 &ov1, const Vector3 &ov2, const Vector3 &ov3, const Vector3 &ov4, RenderArea::f_RenderFragmentCallback callback, void* callback_data)
{
    pushDisplacedTriangle(v2, v3, v1, ov2, ov3, ov1, callback, callback_data);
    pushDisplacedTriangle(v4, v1, v3, ov4, ov1, ov3, callback, callback_data);
}
