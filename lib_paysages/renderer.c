#include "renderer.h"

#include "lighting.h"
#include "system.h"
#include "render.h"
#include "scenery.h"

RayCastingResult _RAYCASTING_NULL = {0};
HeightInfo _WATER_HEIGHT_INFO = {-1000000.0, -1000000.0, -1000000.0};

static void* _renderFirstPass(void* data)
{
    Renderer* renderer = (Renderer*)data;
    
    sceneryRenderFirstPass(renderer);
    renderer->is_rendering = 0;
    return NULL;
}

static int _addRenderProgress(Renderer* renderer, double progress)
{
    return !renderer->render_interrupt;
}

static double _getPrecision(Renderer* renderer, Vector3 location)
{
    return 0.001;
}

static Vector3 _projectPoint(Renderer* renderer, Vector3 point)
{
    return point;
}

static Vector3 _unprojectPoint(Renderer* renderer, Vector3 point)
{
    return point;
}

static void _pushTriangle(Renderer* renderer, Vertex* v1, Vertex* v2, Vertex* v3)
{
    Vector3 p1, p2, p3;
    
    p1 = renderer->projectPoint(renderer, v1->location);
    p2 = renderer->projectPoint(renderer, v2->location);
    p3 = renderer->projectPoint(renderer, v3->location);
    
    renderPushTriangle(renderer->render_area, v1, v2, v3, p1, p2, p3);
}

static void _pushQuad(Renderer* renderer, Vertex* v1, Vertex* v2, Vertex* v3, Vertex* v4)
{
    renderer->pushTriangle(renderer, v2, v3, v1);
    renderer->pushTriangle(renderer, v4, v1, v3);
}

static void _alterLight(Renderer* renderer, LightDefinition* light, Vector3 location)
{
}

static void _getLightStatus(Renderer* renderer, LightStatus* status, Vector3 location)
{
    status->nblights = 0;
}

static Color _applyLightStatus(Renderer* renderer, LightStatus* status, Vector3 location, Vector3 normal, SurfaceMaterial material)
{
    return lightingApplyStatusToSurface(renderer, status, location, normal, material);
}

static RayCastingResult _rayWalking(Renderer* renderer, Vector3 location, Vector3 direction, int terrain, int water, int sky, int clouds)
{
    return _RAYCASTING_NULL;
}

static double _getTerrainHeight(Renderer* renderer, double x, double z)
{
    return 0.0;
}

static HeightInfo _getWaterHeightInfo(Renderer* renderer)
{
    return _WATER_HEIGHT_INFO;
}

static Color _applyTextures(Renderer* renderer, Vector3 location, double precision)
{
    return COLOR_TRANSPARENT;
}

static Color _applyAtmosphere(Renderer* renderer, Vector3 location, Color base)
{
    return base;
}

static Color _applyClouds(Renderer* renderer, Color base, Vector3 start, Vector3 end)
{
    return base;
}

Renderer rendererCreate()
{
    Renderer result;
    RenderParams params = {1, 1, 1, 5};

    result.render_quality = 5;
    result.render_width = 1;
    result.render_height = 1;
    result.render_interrupt = 0;
    result.render_progress = 0.0;
    result.is_rendering = 0;
    result.render_camera = cameraCreateDefinition();
    result.camera_location = result.render_camera.location;
    result.render_area = renderCreateArea();
    
    renderSetParams(result.render_area, params);

    result.addRenderProgress = _addRenderProgress;
    result.getPrecision = _getPrecision;
    result.projectPoint = _projectPoint;
    result.unprojectPoint = _unprojectPoint;
    result.pushTriangle = _pushTriangle;
    result.pushQuad = _pushQuad;

    result.rayWalking = _rayWalking;
    result.getTerrainHeight = _getTerrainHeight;
    result.getWaterHeightInfo = _getWaterHeightInfo;
    result.applyTextures = _applyTextures;
    result.applyAtmosphere = _applyAtmosphere;
    result.applyClouds = _applyClouds;

    result.alterLight = _alterLight;
    result.getLightStatus = _getLightStatus;
    result.applyLightStatus = _applyLightStatus;

    return result;
}

void rendererDelete(Renderer* renderer)
{
    renderDeleteArea(renderer->render_area);
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
    
    cameraSetRenderSize(&renderer->render_camera, renderer->render_width, renderer->render_height);
    renderer->camera_location = renderer->render_camera.location;

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
    renderPostProcess(renderer->render_area, renderer, core_count);
    renderer->is_rendering = 0;
}

void rendererInterrupt(Renderer* renderer)
{
    renderer->render_interrupt = 1;
}
