#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "shared/types.h"
#include "shared/functions.h"
#include "shared/constants.h"
#include "shared/globals.h"
#include "shared/system.h"

#include "water.h"
#include "clouds.h"
#include "sky.h"

static int _cpu_count = 1;
static int _is_rendering = 0;

void autoInit()
{
    _cpu_count = (int)sysconf(_SC_NPROCESSORS_ONLN);
    renderSetBackgroundColor(&COLOR_BLACK);

    terrainInit();
    waterInit();
}

void autoSave(char* filepath)
{
    FILE* f = fopen(filepath, "wb");

    texturesSave(f);

    cameraSave(f);
    cloudsSave(f);
    fogSave(f);
    lightingSave(f);
    renderSave(f);
    skySave(f);
    terrainSave(f);
    waterSave(f);

    fclose(f);
}

void autoLoad(char* filepath)
{
    FILE* f = fopen(filepath, "rb");

    texturesLoad(f);

    cameraLoad(f);
    cloudsLoad(f);
    fogLoad(f);
    lightingLoad(f);
    renderLoad(f);
    skyLoad(f);
    terrainLoad(f);
    waterLoad(f);

    fclose(f);
}

void autoSetDaytime(int hour, int minute)
{
    autoSetDaytimeFraction((double)hour / 24.0 + (double)minute / 1440.0);
}

void autoSetDaytimeFraction(double daytime)
{
    SkyDefinition sky;
    ColorGradation grad_sun;
    Color sun;

    daytime = fmod(daytime, 1.0);
    if (daytime < 0.0)
    {
        daytime += 1.0;
    }

    lightingSetSunAngle(0.0, (daytime + 0.25) * M_PI * 2.0);

    grad_sun = colorGradationCreate();
    colorGradationAddRgba(&grad_sun, 0.2, 0.1, 0.1, 0.1, 1.0);
    colorGradationAddRgba(&grad_sun, 0.25, 0.9, 0.5, 0.5, 1.0);
    colorGradationAddRgba(&grad_sun, 0.3, 0.8, 0.8, 0.8, 1.0);
    colorGradationAddRgba(&grad_sun, 0.5, 1.0, 1.0, 1.0, 1.0);
    colorGradationAddRgba(&grad_sun, 0.7, 0.8, 0.8, 0.8, 1.0);
    colorGradationAddRgba(&grad_sun, 0.75, 0.7, 0.6, 0.5, 1.0);
    colorGradationAddRgba(&grad_sun, 0.8, 0.1, 0.1, 0.1, 1.0);
    sun = colorGradationGet(&grad_sun, daytime);
    lightingSetSunColor(sun);

    sky = skyGetDefinition();
    sky.daytime = daytime;
    skySetDefinition(sky);

    fogSetColor(colorGradationGet(&sky.haze_color, daytime));
}

void autoSetRenderQuality(int quality)
{
    WaterQuality water;
    CloudsQuality clouds;

    if (quality < 1)
    {
        quality = 1;
    }
    if (quality > 10)
    {
        quality = 10;
    }

    renderSetQuality(quality);

    terrainSetChunkSize(0.1 / (double)render_quality, 0.05 / (double)render_quality);

    water.detail_boost = 5.0;
    water.force_detail = 0.0;
    waterSetQuality(water);

    clouds.precision = 3.3 - 0.3 * (double)render_quality;
    cloudsSetQuality(clouds);
}

void autoGenRealisticLandscape(int seed)
{
    Texture* tex;
    WaterDefinition water;
    CloudsDefinition cloud;
    SkyDefinition sky;
    int layer;
    HeightModifier* mod;
    Zone* zone;
    NoiseGenerator* noise;

    if (!seed)
    {
        seed = time(NULL);
    }
    srand(seed);

    /* Cloud layer */
    cloud = cloudsCreateDefinition();
    cloud.ymin = 10.0;
    cloud.ycenter = 40.0;
    cloud.ymax = 100.0;
    cloud.color = COLOR_WHITE;
    cloud.color.a = 0.1;
    cloud.scaling = 50.0;
    cloud.coverage = 0.5;
    noiseGenerateBaseNoise(cloud.noise, 262144);
    noiseAddLevelSimple(cloud.noise, 50.0, 0.3);
    noiseAddLevelSimple(cloud.noise, 50.0 / 2.0, 0.2);
    noiseAddLevelSimple(cloud.noise, 50.0 / 4.0, 0.1);
    noiseAddLevelSimple(cloud.noise, 50.0 / 10.0, 0.05);
    noiseAddLevelSimple(cloud.noise, 50.0 / 20.0, 0.03);
    noiseAddLevelSimple(cloud.noise, 50.0 / 40.0, 0.02);
    noiseAddLevelSimple(cloud.noise, 50.0 / 60.0, 0.01);
    noiseAddLevelSimple(cloud.noise, 50.0 / 80.0, 0.005);
    noiseAddLevelSimple(cloud.noise, 50.0 / 100.0, 0.02);
    noiseAddLevelSimple(cloud.noise, 50.0 / 150.0, 0.005);
    noiseAddLevelSimple(cloud.noise, 50.0 / 200.0, 0.003);
    noiseAddLevelSimple(cloud.noise, 50.0 / 400.0, 0.008);
    noiseAddLevelSimple(cloud.noise, 50.0 / 800.0, 0.001);
    noiseAddLevelSimple(cloud.noise, 50.0 / 1000.0, 0.0005);
    layer = cloudsAddLayer();
    cloudsSetDefinition(layer, cloud);

    /* Water */
    water.height = 0.0;
    water.transparency = 0.4;
    water.reflection = 0.5;
    water.main_color.r = 0.1;
    water.main_color.g = 0.3;
    water.main_color.b = 0.4;
    water.main_color.a = 1.0;
    water.height_noise = noiseCreateGenerator();
    noiseGenerateBaseNoise(water.height_noise, 262144);
    noiseAddLevelsSimple(water.height_noise, 2, 0.2, 0.015);
    noiseAddLevelsSimple(water.height_noise, 3, 0.03, 0.003);
    waterSetDefinition(water);
    noiseDeleteGenerator(water.height_noise);

    /* Sky */
    sky.sun_color = colorGradationCreate();
    sky.zenith_color = colorGradationCreate();
    colorGradationAddRgba(&sky.zenith_color, 0.2, 0.03, 0.03, 0.05, 1.0);
    colorGradationAddRgba(&sky.zenith_color, 0.25, 0.25, 0.33, 0.37, 1.0);
    colorGradationAddRgba(&sky.zenith_color, 0.35, 0.52, 0.63, 0.8, 1.0);
    colorGradationAddRgba(&sky.zenith_color, 0.65, 0.52, 0.63, 0.8, 1.0);
    colorGradationAddRgba(&sky.zenith_color, 0.75, 0.25, 0.33, 0.37, 1.0);
    colorGradationAddRgba(&sky.zenith_color, 0.8, 0.03, 0.03, 0.05, 1.0);
    sky.haze_color = colorGradationCreate();
    colorGradationAddRgba(&sky.haze_color, 0.2, 0.05, 0.05, 0.08, 1.0);
    colorGradationAddRgba(&sky.haze_color, 0.25, 0.55, 0.42, 0.42, 1.0);
    colorGradationAddRgba(&sky.haze_color, 0.3, 0.6, 0.6, 0.6, 1.0);
    colorGradationAddRgba(&sky.haze_color, 0.4, 0.92, 0.93, 1.0, 1.0);
    colorGradationAddRgba(&sky.haze_color, 0.6, 0.92, 0.93, 1.0, 1.0);
    colorGradationAddRgba(&sky.haze_color, 0.7, 0.6, 0.6, 0.8, 1.0);
    colorGradationAddRgba(&sky.haze_color, 0.75, 0.62, 0.50, 0.42, 1.0);
    colorGradationAddRgba(&sky.haze_color, 0.8, 0.05, 0.05, 0.08, 1.0);
    sky.daytime = 0.0;
    sky.haze_height = 0.75;
    sky.haze_smoothing = 0.3;
    skySetDefinition(sky);

    noise = noiseCreateGenerator();
    noiseGenerateBaseNoise(noise, 1048576);
    noiseAddLevelsSimple(noise, 10, 10.0, 1.0);
    noiseNormalizeHeight(noise, -12.0, 12.0, 0);
    terrainSetNoiseGenerator(noise);
    noiseDeleteGenerator(noise);

    tex = textureCreateFromFile("./data/textures/rock3.jpg");
    tex->scaling_x = 0.003;
    tex->scaling_y = 0.003;
    tex->scaling_z = 0.003;
    zone = zoneCreate(1.0);
    terrainAddTexture(tex, 0.05, zone, 1.0);

    tex = textureCreateFromFile("./data/textures/grass1.jpg");
    tex->scaling_x = 0.0004;
    tex->scaling_y = 0.0004;
    tex->scaling_z = 0.0004;
    zone = zoneCreate(0.0);
    zoneAddHeightRange(zone, 1.0, -1.0, 0.0, 3.0, 15.0);
    zoneAddSteepnessRange(zone, 1.0, 0.0, 0.0, 0.3, 0.4);
    terrainAddTexture(tex, 0.15, zone, 0.05);

    /*tex = textureCreateFromFile("./data/textures/snow1.jpg");
    tex->scaling_x = 0.001;
    tex->scaling_y = 0.001;
    tex->scaling_z = 0.001;
    zone = zoneCreate(0.0);
    zoneAddHeightRange(zone, 1.0, 3.0, 4.0, 100.0, 100.0);
    terrainAddTexture(tex, 0.5, zone, 0.1);*/

    /* DEBUG */
    mod = modifierCreate();
    zone = modifierGetZone(mod);
    zoneIncludeCircleArea(zone, 0.4, 0.0, 0.0, 8.0, 20.0);
    modifierActionFixValue(mod, -2.0);
    terrainAddModifier(mod);
    mod = modifierCreate();
    zone = modifierGetZone(mod);
    zoneIncludeCircleArea(zone, 1.0, 0.0, 0.0, 0.3, 8.0);
    modifierActionAddValue(mod, 8.0);
    terrainAddModifier(mod);
    mod = modifierCreate();
    zone = modifierGetZone(mod);
    zoneIncludeCircleArea(zone, 0.8, 0.0, 0.0, 0.3, 4.0);
    modifierActionFixValue(mod, -8.0);
    terrainAddModifier(mod);

    fogSetDistance(20.0, 100.0);
}

void* _renderFirstPass(void* data)
{
    if (!renderSetNextProgressStep(0.0, 0.1))
    {
        _is_rendering = 0;
        return NULL;
    }
    skyRender(renderTellProgress);
    if (!renderSetNextProgressStep(0.1, 0.3))
    {
        _is_rendering = 0;
        return NULL;
    }
    terrainRender(renderTellProgress);
    if (!renderSetNextProgressStep(0.3, 0.4))
    {
        _is_rendering = 0;
        return NULL;
    }
    waterRender(renderTellProgress);
    _is_rendering = 0;
    return NULL;
}

void autoRenderSceneTwoPass(int postonly)
{
    Thread* thread;
    int loops;

    if (!postonly)
    {
        renderClear();

        _is_rendering = 1;
        thread = threadCreate(_renderFirstPass, NULL);
        loops = 0;

        while (_is_rendering)
        {
            timeSleepMs(100);

            if (++loops >= 10)
            {
                renderUpdate();
                loops = 0;
            }
        }

        threadJoin(thread);
    }
    if (renderSetNextProgressStep(0.4, 1.0))
    {
        renderPostProcess(_cpu_count);
    }
}

static int _postProcessRayTracingOverlay(RenderFragment* fragment)
{
    Vector3 terrain_hit, look;

    look = v3Sub(fragment->vertex.location, camera_location);
    if (!terrainProjectRay(camera_location, look, &terrain_hit, &fragment->vertex.color))
    {
        fragment->vertex.color = skyProjectRay(camera_location, look);
    }

    return 1;
}

void autoRenderSceneRayTracing()
{
    renderClear();
    cameraPushOverlay(COLOR_RED, _postProcessRayTracingOverlay);
    renderUpdate();

    if (renderSetNextProgressStep(0.0, 1.0))
    {
        renderPostProcess(_cpu_count);
    }
}
