#include "renderer.h"

#include "tools/lighting.h"
#include "System.h"
#include "Thread.h"
#include "render.h"
#include "Scenery.h"
#include "tools.h"

static RayCastingResult _RAYCASTING_NULL = {0};

static void* _renderFirstPass(void* data)
{
    Renderer* renderer = (Renderer*)data;

    sceneryRenderFirstPass(renderer);
    renderer->is_rendering = 0;
    return NULL;
}

static int _addRenderProgress(Renderer* renderer, double progress)
{
    UNUSED(progress);
    return !renderer->render_interrupt;
}

static Vector3 _getCameraLocation(Renderer* renderer, Vector3 target)
{
    UNUSED(renderer);
    UNUSED(target);
    return cameraGetLocation(renderer->render_camera);
}

static Vector3 _getCameraDirection(Renderer* renderer, Vector3 target)
{
    UNUSED(renderer);
    UNUSED(target);

    return cameraGetDirectionNormalized(renderer->render_camera);
}

static double _getPrecision(Renderer* renderer, Vector3 location)
{
    UNUSED(renderer);
    UNUSED(location);
    return 0.0;
}

static Vector3 _projectPoint(Renderer* renderer, Vector3 point)
{
    return cameraProject(renderer->render_camera, point);
}

static Vector3 _unprojectPoint(Renderer* renderer, Vector3 point)
{
    return cameraUnproject(renderer->render_camera, point);
}

static void _pushTriangle(Renderer* renderer, Vector3 v1, Vector3 v2, Vector3 v3, f_RenderFragmentCallback callback, void* callback_data)
{
    Vector3 p1, p2, p3;

    p1 = renderer->projectPoint(renderer, v1);
    p2 = renderer->projectPoint(renderer, v2);
    p3 = renderer->projectPoint(renderer, v3);

    renderPushTriangle(renderer->render_area, p1, p2, p3, v1, v2, v3, callback, callback_data);
}

static void _pushQuad(Renderer* renderer, Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, f_RenderFragmentCallback callback, void* callback_data)
{
    renderer->pushTriangle(renderer, v2, v3, v1, callback, callback_data);
    renderer->pushTriangle(renderer, v4, v1, v3, callback, callback_data);
}

static void _pushDisplacedTriangle(Renderer* renderer, Vector3 v1, Vector3 v2, Vector3 v3, Vector3 ov1, Vector3 ov2, Vector3 ov3, f_RenderFragmentCallback callback, void* callback_data)
{
    Vector3 p1, p2, p3;

    p1 = renderer->projectPoint(renderer, v1);
    p2 = renderer->projectPoint(renderer, v2);
    p3 = renderer->projectPoint(renderer, v3);

    renderPushTriangle(renderer->render_area, p1, p2, p3, ov1, ov2, ov3, callback, callback_data);
}

static void _pushDisplacedQuad(Renderer* renderer, Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, Vector3 ov1, Vector3 ov2, Vector3 ov3, Vector3 ov4, f_RenderFragmentCallback callback, void* callback_data)
{
    renderer->pushDisplacedTriangle(renderer, v2, v3, v1, ov2, ov3, ov1, callback, callback_data);
    renderer->pushDisplacedTriangle(renderer, v4, v1, v3, ov4, ov1, ov3, callback, callback_data);
}

static RayCastingResult _rayWalking(Renderer* renderer, Vector3 location, Vector3 direction, int terrain, int water, int sky, int clouds)
{
    UNUSED(renderer);
    UNUSED(location);
    UNUSED(direction);
    UNUSED(terrain);
    UNUSED(water);
    UNUSED(sky);
    UNUSED(clouds);
    return _RAYCASTING_NULL;
}

static Color _applyLightingToSurface(Renderer* renderer, Vector3 location, Vector3 normal, SurfaceMaterial* material)
{
    LightStatus* light = lightingCreateStatus(renderer->lighting, location, renderer->getCameraLocation(renderer, location));
    renderer->atmosphere->getLightingStatus(renderer, light, normal, 0);
    Color result = lightingApplyStatus(light, normal, material);
    lightingDeleteStatus(light);
    return result;
}

static Color _applyMediumTraversal(Renderer* renderer, Vector3 location, Color color)
{
    color = renderer->atmosphere->applyAerialPerspective(renderer, location, color).final;
    color = renderer->clouds->getColor(renderer, color, renderer->getCameraLocation(renderer, location), location);
    return color;
}

Renderer* rendererCreate()
{
    Renderer* result = new Renderer;
    RenderParams params = {1, 1, 1, 5};

    result->render_quality = 5;
    result->render_width = 1;
    result->render_height = 1;
    result->render_interrupt = 0;
    result->render_progress = 0.0;
    result->is_rendering = 0;
    result->render_camera = cameraCreateDefinition();
    result->render_area = renderCreateArea(result);

    renderSetParams(result->render_area, params);

    result->addRenderProgress = _addRenderProgress;
    result->getCameraLocation = _getCameraLocation;
    result->getCameraDirection = _getCameraDirection;
    result->getPrecision = _getPrecision;
    result->projectPoint = _projectPoint;
    result->unprojectPoint = _unprojectPoint;
    result->pushTriangle = _pushTriangle;
    result->pushQuad = _pushQuad;
    result->pushDisplacedTriangle = _pushDisplacedTriangle;
    result->pushDisplacedQuad = _pushDisplacedQuad;

    result->rayWalking = _rayWalking;

    result->applyLightingToSurface = _applyLightingToSurface;
    result->applyMediumTraversal = _applyMediumTraversal;

    result->lighting = lightingManagerCreate();

    result->atmosphere = (AtmosphereRenderer*)AtmosphereRendererClass.create();
    result->clouds = (CloudsRenderer*)CloudsRendererClass.create();
    result->terrain = (TerrainRenderer*)TerrainRendererClass.create();
    result->textures = (TexturesRenderer*)TexturesRendererClass.create();
    result->water = (WaterRenderer*)WaterRendererClass.create();

    return result;
}

void rendererDelete(Renderer* renderer)
{
    cameraDeleteDefinition(renderer->render_camera);
    lightingManagerDelete(renderer->lighting);

    AtmosphereRendererClass.destroy(renderer->atmosphere);
    CloudsRendererClass.destroy(renderer->clouds);
    TerrainRendererClass.destroy(renderer->terrain);
    TexturesRendererClass.destroy(renderer->textures);
    WaterRendererClass.destroy(renderer->water);

    renderDeleteArea(renderer->render_area);

    delete renderer;
}

void rendererSetPreviewCallbacks(Renderer* renderer, RenderCallbackStart start, RenderCallbackDraw draw, RenderCallbackUpdate update)
{
    renderSetPreviewCallbacks(renderer->render_area, start, draw, update);
}

void rendererStart(Renderer* renderer, RenderParams params)
{
    Thread* thread;
    int loops;
    int core_count = systemGetCoreCount();

    params.antialias = (params.antialias < 1) ? 1 : params.antialias;
    params.antialias = (params.antialias > 4) ? 4 : params.antialias;

    renderer->render_quality = params.quality;
    renderer->render_width = params.width * params.antialias;
    renderer->render_height = params.height * params.antialias;
    renderer->render_interrupt = 0;
    renderer->render_progress = 0.0;

    cameraSetRenderSize(renderer->render_camera, renderer->render_width, renderer->render_height);

    renderSetBackgroundColor(renderer->render_area, &COLOR_BLACK);
    renderSetParams(renderer->render_area, params);
    renderClear(renderer->render_area);

    renderer->is_rendering = 1;
    thread = threadCreate(_renderFirstPass, renderer);
    loops = 0;

    while (renderer->is_rendering)
    {
        timeSleepMs(100);

        if (++loops >= 10)
        {

            renderUpdate(renderer->render_area);
            loops = 0;
        }
    }
    threadJoin(thread);

    renderer->is_rendering = 1;
    renderPostProcess(renderer->render_area, core_count);
    renderer->is_rendering = 0;
}

void rendererInterrupt(Renderer* renderer)
{
    renderer->render_interrupt = 1;
}
