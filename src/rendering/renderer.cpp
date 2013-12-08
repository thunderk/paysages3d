#include "renderer.h"

#include "System.h"
#include "Thread.h"
#include "render.h"
#include "RenderingScenery.h"
#include "CameraDefinition.h"
#include "SurfaceMaterial.h"

static RayCastingResult _RAYCASTING_NULL = {0, {0.0, 0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};

static void* _renderFirstPass(void* data)
{
    Renderer* renderer = (Renderer*)data;

    renderer->rasterize();
    renderer->is_rendering = 0;
    return NULL;
}

static int _addRenderProgress(Renderer* renderer, double)
{
    return !renderer->render_interrupt;
}

static Vector3 _getCameraLocation(Renderer* renderer, Vector3)
{
    return renderer->render_camera->getLocation();
}

static Vector3 _getCameraDirection(Renderer* renderer, Vector3)
{
    return renderer->render_camera->getDirectionNormalized();
}

static double _getPrecision(Renderer*, Vector3)
{
    return 0.0;
}

static Vector3 _projectPoint(Renderer* renderer, Vector3 point)
{
    return renderer->render_camera->project(point);
}

static Vector3 _unprojectPoint(Renderer* renderer, Vector3 point)
{
    return renderer->render_camera->unproject(point);
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





Renderer::Renderer()
{
    RenderParams params = {1, 1, 1, 5};

    render_quality = 5;
    render_width = 1;
    render_height = 1;
    render_interrupt = 0;
    render_progress = 0.0;
    is_rendering = 0;
    render_camera = new CameraDefinition;
    render_area = renderCreateArea(this);

    renderSetParams(render_area, params);

    addRenderProgress = _addRenderProgress;
    getCameraLocation = _getCameraLocation;
    getCameraDirection = _getCameraDirection;
    getPrecision = _getPrecision;
    projectPoint = _projectPoint;
    unprojectPoint = _unprojectPoint;
    pushTriangle = _pushTriangle;
    pushQuad = _pushQuad;
    pushDisplacedTriangle = _pushDisplacedTriangle;
    pushDisplacedQuad = _pushDisplacedQuad;
}

Renderer::~Renderer()
{
    delete render_camera;

    renderDeleteArea(render_area);
}

Color Renderer::applyMediumTraversal(Vector3, Color color)
{
    return color;
}

Color Renderer::applyLightingToSurface(const Vector3 &, const Vector3 &, const SurfaceMaterial &material)
{
    return material._rgb;
}

RayCastingResult Renderer::rayWalking(const Vector3 &, const Vector3 &, int, int, int, int)
{
    return _RAYCASTING_NULL;
}









// Old API compat

Renderer* rendererCreate()
{
    return new Renderer();
}

void rendererDelete(Renderer* renderer)
{
    delete renderer;
}

void rendererSetPreviewCallbacks(Renderer* renderer, RenderCallbackStart start, RenderCallbackDraw draw, RenderCallbackUpdate update)
{
    renderSetPreviewCallbacks(renderer->render_area, start, draw, update);
}

void rendererStart(Renderer* renderer, RenderParams params)
{
    Thread thread(_renderFirstPass);
    int loops;
    int core_count = System::getCoreCount();

    params.antialias = (params.antialias < 1) ? 1 : params.antialias;
    params.antialias = (params.antialias > 4) ? 4 : params.antialias;

    renderer->render_quality = params.quality;
    renderer->render_width = params.width * params.antialias;
    renderer->render_height = params.height * params.antialias;
    renderer->render_interrupt = 0;
    renderer->render_progress = 0.0;

    renderer->render_camera->setRenderSize(renderer->render_width, renderer->render_height);

    renderSetBackgroundColor(renderer->render_area, COLOR_BLACK);
    renderSetParams(renderer->render_area, params);
    renderClear(renderer->render_area);

    renderer->prepare();

    renderer->is_rendering = 1;
    thread.start(renderer);
    loops = 0;

    while (renderer->is_rendering)
    {
        Thread::timeSleepMs(100);

        if (++loops >= 10)
        {

            renderUpdate(renderer->render_area);
            loops = 0;
        }
    }
    thread.join();

    renderer->is_rendering = 1;
    renderPostProcess(renderer->render_area, core_count);
    renderer->is_rendering = 0;
}

void rendererInterrupt(Renderer* renderer)
{
    renderer->render_interrupt = 1;
}
