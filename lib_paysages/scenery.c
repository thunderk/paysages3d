#include "scenery.h"

#include <stdio.h>

CameraDefinition _camera;
CloudsDefinition _clouds;
LightingDefinition _lighting;
SkyDefinition _sky;
TerrainDefinition _terrain;
TexturesDefinition _textures;
WaterDefinition _water;

static Color _filterLight(Renderer* renderer, Color light_color, Vector3 at_location, Vector3 light_location, Vector3 direction_to_light)
{
    Color result;

    result = waterLightFilter(&_water, renderer, light_color, at_location, light_location, direction_to_light);
    result = terrainLightFilter(&_terrain, renderer, result, at_location, light_location, direction_to_light);
    // TODO atmosphere filter
    // TODO clouds filter

    return result;
}

static RayCastingResult _reflectionFunction(Vector3 start, Vector3 direction)
{
    RayCastingResult result;

    if (!terrainProjectRay(start, direction, &result.hit_location, &result.hit_color))
    {
        result.hit_color = skyProjectRay(start, direction);
        /* TODO hit_location */
    }

    result.hit = 1;
    return result;
}

static RayCastingResult _refractionFunction(Vector3 start, Vector3 direction)
{
    RayCastingResult result;

    result.hit = terrainProjectRay(start, direction, &result.hit_location, &result.hit_color);

    return result;
}

void sceneryInit()
{
    cameraInit();
    cloudsInit();
    lightingInit();
    skyInit();
    terrainInit();
    texturesInit();
    waterInit();

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
    cloudsCopyDefinition(_clouds, clouds);
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

Renderer sceneryGetStandardRenderer()
{
}

void sceneryRender()
{
}
