#include "scenery.h"

#include <stdio.h>
#include "color.h"
#include "euclid.h"
#include "render.h"
#include "system.h"
#include "vegetation.h"

static AtmosphereDefinition _atmosphere;
static CameraDefinition _camera;
static CloudsDefinition _clouds;
static LightingDefinition _lighting;
static SkyDefinition _sky;
static TerrainDefinition _terrain;
static TexturesDefinition _textures;
static VegetationDefinition* _vegetation;
static WaterDefinition _water;

static SceneryCustomDataCallback _custom_save = NULL;
static SceneryCustomDataCallback _custom_load = NULL;
static void* _custom_data = NULL;

void sceneryInit()
{
    noiseInit();
    atmosphereInit();
    cameraInit();
    cloudsInit();
    lightingInit();
    skyInit();
    terrainInit();
    texturesInit();
    vegetationInit();
    waterInit();

    _atmosphere = atmosphereCreateDefinition();
    _camera = cameraCreateDefinition();
    _clouds = cloudsCreateDefinition();
    _lighting = lightingCreateDefinition();
    _sky = skyCreateDefinition();
    _terrain = terrainCreateDefinition();
    _textures = texturesCreateDefinition();
    _vegetation = vegetationCreateDefinition();
    _water = waterCreateDefinition();
    
    _custom_save = NULL;
    _custom_load = NULL;
}

void sceneryQuit()
{
    atmosphereDeleteDefinition(&_atmosphere);
    cameraDeleteDefinition(&_camera);
    cloudsDeleteDefinition(&_clouds);
    lightingDeleteDefinition(&_lighting);
    skyDeleteDefinition(&_sky);
    terrainDeleteDefinition(&_terrain);
    texturesDeleteDefinition(&_textures);
    vegetationDeleteDefinition(_vegetation);
    waterDeleteDefinition(&_water);

    atmosphereQuit();
    cameraQuit();
    cloudsQuit();
    lightingQuit();
    skyQuit();
    terrainQuit();
    texturesQuit();
    vegetationQuit();
    waterQuit();
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
    atmosphereSave(stream, &_atmosphere);
    cameraSave(stream, &_camera);
    cloudsSave(stream, &_clouds);
    lightingSave(stream, &_lighting);
    skySave(stream, &_sky);
    terrainSave(stream, &_terrain);
    texturesSave(stream, &_textures);
    vegetationSave(stream, _vegetation);
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
    atmosphereLoad(stream, &_atmosphere);
    cameraLoad(stream, &_camera);
    cloudsLoad(stream, &_clouds);
    lightingLoad(stream, &_lighting);
    skyLoad(stream, &_sky);
    terrainLoad(stream, &_terrain);
    texturesLoad(stream, &_textures);
    vegetationLoad(stream, _vegetation);
    waterLoad(stream, &_water);
    
    atmosphereValidateDefinition(&_atmosphere);
    cameraValidateDefinition(&_camera, 0);
    cloudsValidateDefinition(&_clouds);
    lightingValidateDefinition(&_lighting);
    skyValidateDefinition(&_sky);
    terrainValidateDefinition(&_terrain);
    texturesValidateDefinition(&_textures);
    vegetationValidateDefinition(_vegetation);
    waterValidateDefinition(&_water);
    
    if (_custom_load)
    {
        _custom_load(stream, _custom_data);
    }
}

void scenerySetAtmosphere(AtmosphereDefinition* atmosphere)
{
    atmosphereCopyDefinition(atmosphere, &_atmosphere);
    atmosphereValidateDefinition(&_atmosphere);
}

void sceneryGetAtmosphere(AtmosphereDefinition* atmosphere)
{
    atmosphereCopyDefinition(&_atmosphere, atmosphere);
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

void scenerySetLighting(LightingDefinition* lighting)
{
    lightingCopyDefinition(lighting, &_lighting);
    lightingValidateDefinition(&_lighting);
}

void sceneryGetLighting(LightingDefinition* lighting)
{
    lightingCopyDefinition(&_lighting, lighting);
}

void scenerySetSky(SkyDefinition* sky)
{
    skyCopyDefinition(sky, &_sky);
    skyValidateDefinition(&_sky);

    atmosphereValidateDefinition(&_atmosphere);
    lightingValidateDefinition(&_lighting);
}

void sceneryGetSky(SkyDefinition* sky)
{
    skyCopyDefinition(&_sky, sky);
}

void scenerySetTerrain(TerrainDefinition* terrain)
{
    terrainCopyDefinition(terrain, &_terrain);
    terrainValidateDefinition(&_terrain);
    
    cameraValidateDefinition(&_camera, 1);
}

void sceneryGetTerrain(TerrainDefinition* terrain)
{
    terrainCopyDefinition(&_terrain, terrain);
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

void scenerySetVegetation(VegetationDefinition* vegetation)
{
    vegetationCopyDefinition(vegetation, _vegetation);
    vegetationValidateDefinition(_vegetation);
}

void sceneryGetTVegetation(VegetationDefinition* vegetation)
{
    vegetationCopyDefinition(_vegetation, vegetation);
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
    terrainRender(&_terrain, renderer);
    waterRender(&_water, renderer);
    skyRender(&_sky, renderer);
}






/******* Standard renderer *********/
static int _getSkyDomeLights(Renderer* renderer, LightDefinition* array, int max_lights)
{
    return skyGetLights(&_sky, renderer, array, max_lights);
}

static void _alterLight(Renderer* renderer, LightDefinition* light, Vector3 location)
{
    Vector3 light_location;
    Vector3 direction_to_light;
    
    direction_to_light = v3Normalize(v3Scale(light->direction, -1.0));
    light_location = v3Add(location, v3Scale(direction_to_light, 1000.0));
    
    if (light->filtered)
    {
        // TODO atmosphere filter
        light->color = waterLightFilter(&_water, renderer, light->color, location, light_location, direction_to_light);
    }
    if (light->masked)
    {
        light->color = terrainLightFilter(&_terrain, renderer, light->color, location, light_location, direction_to_light);
        light->color = cloudsFilterLight(&_clouds, renderer, light->color, location, light_location, direction_to_light);
    }
}

static void _getLightStatus(Renderer* renderer, LightStatus* status, Vector3 location)
{
    lightingGetStatus(&_lighting, renderer, location, status);
}

static Color _applyLightStatus(Renderer* renderer, LightStatus* status, Vector3 location, Vector3 normal, SurfaceMaterial material)
{
    return lightingApplyStatusToSurface(renderer, status, location, normal, material);
}

static RayCastingResult _rayWalking(Renderer* renderer, Vector3 location, Vector3 direction, int terrain, int water, int sky, int clouds)
{
    RayCastingResult result;
    Color sky_color;

    if (!terrainProjectRay(&_terrain, renderer, location, direction, &result.hit_location, &result.hit_color))
    {
        sky_color = skyGetColor(&_sky, renderer, location, direction);
        result.hit_location = v3Add(location, v3Scale(direction, 1000.0));
        result.hit_color = renderer->applyClouds(renderer, sky_color, location, result.hit_location);
    }

    result.hit = 1;
    return result;
}

static double _getTerrainHeight(Renderer* renderer, double x, double z)
{
    return terrainGetHeight(&_terrain, x, z);
}

static HeightInfo _getWaterHeightInfo(Renderer* renderer)
{
    return waterGetHeightInfo(&_water);
}

static Color _applyTextures(Renderer* renderer, Vector3 location, double precision)
{
    return texturesGetColor(&_textures, renderer, location.x, location.z, precision);
}

static Color _applyAtmosphere(Renderer* renderer, Vector3 location, Color base)
{
    return atmosphereApply(&_atmosphere, renderer, location, base);
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

    result.getSkyDomeLights = _getSkyDomeLights;
    result.alterLight = _alterLight;
    result.getLightStatus = _getLightStatus;
    result.applyLightStatus = _applyLightStatus;
    result.rayWalking = _rayWalking;
    result.getTerrainHeight = _getTerrainHeight;
    result.getWaterHeightInfo = _getWaterHeightInfo;
    result.applyTextures = _applyTextures;
    result.applyAtmosphere = _applyAtmosphere;
    result.applyClouds = _applyClouds;
    result.projectPoint = _projectPoint;
    result.unprojectPoint = _unprojectPoint;
    result.getPrecision = _getPrecision;

    return result;
}

