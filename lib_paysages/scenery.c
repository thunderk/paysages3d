#include "scenery.h"

#include "tools/color.h"
#include "tools/euclid.h"
#include "render.h"
#include "system.h"

static AtmosphereDefinition* _atmosphere;
static CameraDefinition _camera;
static CloudsDefinition _clouds;
static TerrainDefinition* _terrain;
static TexturesDefinition _textures;
static WaterDefinition _water;

static SceneryCustomDataCallback _custom_save = NULL;
static SceneryCustomDataCallback _custom_load = NULL;
static void* _custom_data = NULL;

void sceneryInit()
{
    noiseInit();

    _atmosphere = AtmosphereDefinitionClass.create();
    _camera = cameraCreateDefinition();
    _clouds = cloudsCreateDefinition();
    _terrain = TerrainDefinitionClass.create();
    _textures = texturesCreateDefinition();
    _water = waterCreateDefinition();

    _custom_save = NULL;
    _custom_load = NULL;
}

void sceneryQuit()
{
    AtmosphereDefinitionClass.destroy(_atmosphere);
    cameraDeleteDefinition(&_camera);
    cloudsDeleteDefinition(&_clouds);
    TerrainDefinitionClass.destroy(_terrain);
    texturesDeleteDefinition(&_textures);
    waterDeleteDefinition(&_water);

    noiseQuit();
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
    cameraSave(stream, &_camera);
    cloudsSave(stream, &_clouds);
    TerrainDefinitionClass.save(stream, _terrain);
    texturesSave(stream, &_textures);
    waterSave(stream, &_water);

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
    cameraLoad(stream, &_camera);
    cloudsLoad(stream, &_clouds);
    TerrainDefinitionClass.load(stream, _terrain);
    texturesLoad(stream, &_textures);
    waterLoad(stream, &_water);

    cameraValidateDefinition(&_camera, 0);
    cloudsValidateDefinition(&_clouds);
    texturesValidateDefinition(&_textures);
    waterValidateDefinition(&_water);

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
    cameraCopyDefinition(camera, &_camera);
    cameraValidateDefinition(&_camera, 1);
}

void sceneryGetCamera(CameraDefinition* camera)
{
    cameraCopyDefinition(&_camera, camera);
}

void scenerySetClouds(CloudsDefinition* clouds)
{
    cloudsCopyDefinition(clouds, &_clouds);
    cloudsValidateDefinition(&_clouds);
}

void sceneryGetClouds(CloudsDefinition* clouds)
{
    cloudsCopyDefinition(&_clouds, clouds);
}

void scenerySetTerrain(TerrainDefinition* terrain)
{
    TerrainDefinitionClass.copy(terrain, _terrain);

    cameraValidateDefinition(&_camera, 1);
}

void sceneryGetTerrain(TerrainDefinition* terrain)
{
    TerrainDefinitionClass.copy(_terrain, terrain);
}

void scenerySetTextures(TexturesDefinition* textures)
{
    texturesCopyDefinition(textures, &_textures);
    texturesValidateDefinition(&_textures);
}

void sceneryGetTextures(TexturesDefinition* textures)
{
    texturesCopyDefinition(&_textures, textures);
}

void scenerySetWater(WaterDefinition* water)
{
    waterCopyDefinition(water, &_water);
    waterValidateDefinition(&_water);

    cameraValidateDefinition(&_camera, 1);
}

void sceneryGetWater(WaterDefinition* water)
{
    waterCopyDefinition(&_water, water);
}

void sceneryRenderFirstPass(Renderer* renderer)
{
    terrainRenderSurface(renderer);
    waterRender(&_water, renderer);
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
        sky_color = renderer->atmosphere->getSkyColor(renderer, direction);

        result.hit = 1;
        result.hit_location = v3Add(location, v3Scale(direction, 1000.0));
        result.hit_color = renderer->applyClouds(renderer, sky_color, location, result.hit_location);
    }

    return result;
}

static HeightInfo _getWaterHeightInfo(Renderer* renderer)
{
    return waterGetHeightInfo(&_water);
}

static Color _applyTextures(Renderer* renderer, Vector3 location, double precision)
{
    return texturesGetColor(&_textures, renderer, location.x, location.z, precision);
}

static Color _applyClouds(Renderer* renderer, Color base, Vector3 start, Vector3 end)
{
    return cloudsApply(&_clouds, base, renderer, start, end);
}

static Vector3 _projectPoint(Renderer* renderer, Vector3 point)
{
    return cameraProject(&renderer->render_camera, renderer, point);
}

static Vector3 _unprojectPoint(Renderer* renderer, Vector3 point)
{
    return cameraUnproject(&renderer->render_camera, renderer, point);
}

static double _getPrecision(Renderer* renderer, Vector3 location)
{
    Vector3 projected;

    projected = cameraProject(&renderer->render_camera, renderer, location);
    projected.x += 1.0;
    //projected.y += 1.0;

    return v3Norm(v3Sub(cameraUnproject(&renderer->render_camera, renderer, projected), location)); // / (double)render_quality;
}

Renderer sceneryCreateStandardRenderer()
{
    Renderer result;

    result = rendererCreate();

    cameraCopyDefinition(&_camera, &result.render_camera);
    result.camera_location = _camera.location;

    result.rayWalking = _rayWalking;
    result.getWaterHeightInfo = _getWaterHeightInfo;
    result.applyTextures = _applyTextures;
    result.applyClouds = _applyClouds;
    result.projectPoint = _projectPoint;
    result.unprojectPoint = _unprojectPoint;
    result.getPrecision = _getPrecision;

    AtmosphereRendererClass.bind(result.atmosphere, _atmosphere);
    TerrainRendererClass.bind(result.terrain, _terrain);

    return result;
}

