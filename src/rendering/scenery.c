#include "scenery.h"

#include "tools/color.h"
#include "tools/euclid.h"
#include "render.h"
#include "system.h"

static AtmosphereDefinition* _atmosphere;
static CameraDefinition* _camera;
static CloudsDefinition* _clouds;
static TerrainDefinition* _terrain;
static TexturesDefinition* _textures;
static WaterDefinition* _water;

static SceneryCustomDataCallback _custom_save = NULL;
static SceneryCustomDataCallback _custom_load = NULL;
static void* _custom_data = NULL;

void sceneryInit()
{
    noiseInit();

    _atmosphere = AtmosphereDefinitionClass.create();
    _camera = cameraCreateDefinition();
    _clouds = CloudsDefinitionClass.create();
    _terrain = TerrainDefinitionClass.create();
    _textures = TexturesDefinitionClass.create();
    _water = WaterDefinitionClass.create();

    _custom_save = NULL;
    _custom_load = NULL;
}

void sceneryQuit()
{
    AtmosphereDefinitionClass.destroy(_atmosphere);
    cameraDeleteDefinition(_camera);
    CloudsDefinitionClass.destroy(_clouds);
    TerrainDefinitionClass.destroy(_terrain);
    TexturesDefinitionClass.destroy(_textures);
    AtmosphereDefinitionClass.destroy(_water);

    noiseQuit();
}

void sceneryAutoPreset()
{
    terrainAutoPreset(_terrain, TERRAIN_PRESET_STANDARD);
    texturesAutoPreset(_textures, TEXTURES_PRESET_IRELAND);
    atmosphereAutoPreset(_atmosphere, ATMOSPHERE_PRESET_CLEAR_DAY);
    waterAutoPreset(_water, WATER_PRESET_LAKE);
    cloudsAutoPreset(_clouds, CLOUDS_PRESET_PARTLY_CLOUDY);
}

void scenerySetCustomDataCallback(SceneryCustomDataCallback callback_save, SceneryCustomDataCallback callback_load, void* data)
{
    _custom_save = callback_save;
    _custom_load = callback_load;
    _custom_data = data;
}

void scenerySave(PackStream* stream)
{
    noiseSave(stream);
    AtmosphereDefinitionClass.save(stream, _atmosphere);
    cameraSave(stream, _camera);
    CloudsDefinitionClass.save(stream, _clouds);
    TerrainDefinitionClass.save(stream, _terrain);
    TexturesDefinitionClass.save(stream, _textures);
    WaterDefinitionClass.save(stream, _water);

    if (_custom_save)
    {
        _custom_save(stream, _custom_data);
    }
}

void sceneryLoad(PackStream* stream)
{
    /* TODO Use intermediary definitions ? */

    noiseLoad(stream);
    AtmosphereDefinitionClass.load(stream, _atmosphere);
    cameraLoad(stream, _camera);
    CloudsDefinitionClass.load(stream, _clouds);
    TerrainDefinitionClass.load(stream, _terrain);
    TexturesDefinitionClass.load(stream, _textures);
    WaterDefinitionClass.load(stream, _water);

    if (_custom_load)
    {
        _custom_load(stream, _custom_data);
    }
}

void scenerySetAtmosphere(AtmosphereDefinition* atmosphere)
{
    AtmosphereDefinitionClass.copy(atmosphere, _atmosphere);
}

void sceneryGetAtmosphere(AtmosphereDefinition* atmosphere)
{
    AtmosphereDefinitionClass.copy(_atmosphere, atmosphere);
}

void scenerySetCamera(CameraDefinition* camera)
{
    cameraCopyDefinition(camera, _camera);
    cameraValidateDefinition(_camera, 1);
}

void sceneryGetCamera(CameraDefinition* camera)
{
    cameraCopyDefinition(_camera, camera);
}

void scenerySetClouds(CloudsDefinition* clouds)
{
    CloudsDefinitionClass.copy(clouds, _clouds);
}

void sceneryGetClouds(CloudsDefinition* clouds)
{
    CloudsDefinitionClass.copy(_clouds, clouds);
}

void scenerySetTerrain(TerrainDefinition* terrain)
{
    TerrainDefinitionClass.copy(terrain, _terrain);

    cameraValidateDefinition(_camera, 1);
}

void sceneryGetTerrain(TerrainDefinition* terrain)
{
    TerrainDefinitionClass.copy(_terrain, terrain);
}

void scenerySetTextures(TexturesDefinition* textures)
{
    TexturesDefinitionClass.copy(textures, _textures);

    cameraValidateDefinition(_camera, 1);
}

void sceneryGetTextures(TexturesDefinition* textures)
{
    TexturesDefinitionClass.copy(_textures, textures);
}

void scenerySetWater(WaterDefinition* water)
{
    WaterDefinitionClass.copy(water, _water);

    cameraValidateDefinition(_camera, 1);
}

void sceneryGetWater(WaterDefinition* water)
{
    WaterDefinitionClass.copy(_water, water);
}

void sceneryRenderFirstPass(Renderer* renderer)
{
    terrainRenderSurface(renderer);
    waterRenderSurface(renderer);
    atmosphereRenderSkydome(renderer);
}

/******* Standard renderer *********/
static RayCastingResult _rayWalking(Renderer* renderer, Vector3 location, Vector3 direction, int terrain, int water, int sky, int clouds)
{
    RayCastingResult result;
    Color sky_color;

    result = renderer->terrain->castRay(renderer, location, direction);
    if (!result.hit)
    {
        sky_color = renderer->atmosphere->getSkyColor(renderer, direction).final;

        result.hit = 1;
        result.hit_location = v3Add(location, v3Scale(direction, 1000.0));
        result.hit_color = renderer->clouds->getColor(renderer, sky_color, location, result.hit_location);
    }

    return result;
}

static Vector3 _projectPoint(Renderer* renderer, Vector3 point)
{
    return cameraProject(renderer->render_camera, point);
}

static Vector3 _unprojectPoint(Renderer* renderer, Vector3 point)
{
    return cameraUnproject(renderer->render_camera, point);
}

static double _getPrecision(Renderer* renderer, Vector3 location)
{
    Vector3 projected;

    projected = cameraProject(renderer->render_camera, location);
    projected.x += 1.0;
    //projected.y += 1.0;

    return v3Norm(v3Sub(cameraUnproject(renderer->render_camera, projected), location)); // / (double)render_quality;
}

Renderer* sceneryCreateStandardRenderer()
{
    Renderer* result;

    result = rendererCreate();

    cameraCopyDefinition(_camera, result->render_camera);

    result->rayWalking = _rayWalking;
    result->projectPoint = _projectPoint;
    result->unprojectPoint = _unprojectPoint;
    result->getPrecision = _getPrecision;

    sceneryBindRenderer(result);

    return result;
}

void sceneryBindRenderer(Renderer* renderer)
{
    AtmosphereRendererClass.bind(renderer, _atmosphere);
    TerrainRendererClass.bind(renderer, _terrain);
    TexturesRendererClass.bind(renderer, _textures);
    CloudsRendererClass.bind(renderer, _clouds);
    WaterRendererClass.bind(renderer, _water);
}
