#include "scenery.h"

#include <stdio.h>
#include "shared/functions.h"
#include "system.h"
#include "render.h"

AtmosphereDefinition _atmosphere;
CameraDefinition _camera;
CloudsDefinition _clouds;
LightingDefinition _lighting;
SkyDefinition _sky;
TerrainDefinition _terrain;
TexturesDefinition _textures;
WaterDefinition _water;

void sceneryInit()
{
    atmosphereInit();
    cameraInit();
    cloudsInit();
    lightingInit();
    skyInit();
    terrainInit();
    texturesInit();
    waterInit();

    _atmosphere = atmosphereCreateDefinition();
    _camera = cameraCreateDefinition();
    _clouds = cloudsCreateDefinition();
    _lighting = lightingCreateDefinition();
    _sky = skyCreateDefinition();
    _terrain = terrainCreateDefinition();
    _textures = texturesCreateDefinition();
    _water = waterCreateDefinition();
}

void scenerySaveToFile(char* filepath)
{
    FILE* f = fopen(filepath, "wb");

    atmosphereSave(f, &_atmosphere);
    cameraSave(f, &_camera);
    cloudsSave(f, &_clouds);
    lightingSave(f, &_lighting);
    skySave(f, &_sky);
    terrainSave(f, &_terrain);
    texturesSave(f, &_textures);
    waterSave(f, &_water);

    fclose(f);
}

void sceneryLoadFromFile(char* filepath)
{
    FILE* f = fopen(filepath, "rb");

    /* TODO Use intermediary definitions ? */

    atmosphereLoad(f, &_atmosphere);
    atmosphereValidateDefinition(&_atmosphere);

    cameraLoad(f, &_camera);
    cameraValidateDefinition(&_camera);

    cloudsLoad(f, &_clouds);
    cloudsValidateDefinition(&_clouds);

    lightingLoad(f, &_lighting);
    lightingValidateDefinition(&_lighting);

    skyLoad(f, &_sky);
    skyValidateDefinition(&_sky);

    terrainLoad(f, &_terrain);
    terrainValidateDefinition(&_terrain);

    texturesLoad(f, &_textures);
    texturesValidateDefinition(&_textures);

    waterLoad(f, &_water);
    waterValidateDefinition(&_water);

    fclose(f);
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
    cameraValidateDefinition(&_camera);
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

void scenerySetWater(WaterDefinition* water)
{
    waterCopyDefinition(water, &_water);
    waterValidateDefinition(&_water);
}

void sceneryGetWater(WaterDefinition* water)
{
    waterCopyDefinition(&_water, water);
}

void sceneryRenderFirstPass(Renderer* renderer)
{
    if (!renderSetNextProgressStep(0.0, 0.01))
    {
        return;
    }
    skyRender(&_sky, renderer, renderTellProgress);
    if (!renderSetNextProgressStep(0.01, 0.085))
    {
        return;
    }
    terrainRender(&_terrain, renderer, renderTellProgress);
    if (!renderSetNextProgressStep(0.085, 0.1))
    {
        return;
    }
    waterRender(&_water, renderer, renderTellProgress);
}

void sceneryRenderSecondPass(Renderer* renderer)
{
    if (renderSetNextProgressStep(0.1, 1.0))
    {
        renderPostProcess(renderer, systemGetCoreCount());
    }
}






/******* Standard renderer *********/
static Color _filterLight(Renderer* renderer, Color light_color, Vector3 at_location, Vector3 light_location, Vector3 direction_to_light)
{
    // TODO atmosphere filter
    light_color = waterLightFilter(&_water, renderer, light_color, at_location, light_location, direction_to_light);

    return light_color;
}

static Color _maskLight(Renderer* renderer, Color light_color, Vector3 at_location, Vector3 light_location, Vector3 direction_to_light)
{
    light_color = terrainLightFilter(&_terrain, renderer, light_color, at_location, light_location, direction_to_light);
    light_color = cloudsFilterLight(&_clouds, renderer, light_color, at_location, light_location, direction_to_light);

    return light_color;
}

static Color _applyLightingToSurface(Renderer* renderer, Vector3 location, Vector3 normal, SurfaceMaterial material)
{
    return lightingApplyToSurface(&_lighting, renderer, location, normal, material);
}

static RayCastingResult _rayWalking(Renderer* renderer, Vector3 location, Vector3 direction, int terrain, int water, int sky, int clouds)
{
    RayCastingResult result;

    if (!terrainProjectRay(&_terrain, renderer, location, direction, &result.hit_location, &result.hit_color))
    {
        result.hit_color = skyGetColor(&_sky, renderer, location, direction);
        /* TODO hit_location */
    }

    result.hit = 1;
    return result;
}

static double _getTerrainHeight(Renderer* renderer, double x, double z)
{
    return terrainGetHeight(&_terrain, x, z);
}

static Color _applyTextures(Renderer* renderer, Vector3 location, double precision)
{
    return texturesGetColor(&_textures, renderer, location, precision);
}

static Color _applyAtmosphere(Renderer* renderer, Vector3 location, Color base)
{
    return atmosphereApply(&_atmosphere, renderer, location, base);
}

static Color _applyClouds(Renderer* renderer, Color base, Vector3 start, Vector3 end)
{
    Color clouds;

    clouds = cloudsGetColor(&_clouds, renderer, start, end);
    colorMask(&base, &clouds);

    return base;
}

static Vector3 _projectPoint(Renderer* renderer, Vector3 point)
{
    return cameraProject(&_camera, point);
}

static Vector3 _unprojectPoint(Renderer* renderer, Vector3 point)
{
    return cameraUnproject(&_camera, point);
}

static double _getPrecision(Renderer* renderer, Vector3 location)
{
    Vector3 projected;

    projected = cameraProject(&_camera, location);
    projected.x += 1.0;
    //projected.y += 1.0;

    return v3Norm(v3Sub(cameraUnproject(&_camera, projected), location)); // / (double)render_quality;
}

Renderer sceneryGetStandardRenderer(int quality)
{
    Renderer result;

    quality = (quality > 10) ? 10 : quality;
    quality = (quality < 1) ? 1 : quality;

    result.camera_location = _camera.location;
    result.render_quality = quality;

    result.filterLight = _filterLight;
    result.maskLight = _maskLight;
    result.applyLightingToSurface = _applyLightingToSurface;
    result.rayWalking = _rayWalking;
    result.getTerrainHeight = _getTerrainHeight;
    result.applyTextures = _applyTextures;
    result.applyAtmosphere = _applyAtmosphere;
    result.applyClouds = _applyClouds;
    result.projectPoint = _projectPoint;
    result.unprojectPoint = _unprojectPoint;
    result.getPrecision = _getPrecision;

    return result;
}

