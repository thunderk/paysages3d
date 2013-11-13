#include "Scenery.h"

#include <ctime>

#include "NoiseGenerator.h"
#include "PackStream.h"
#include "atmosphere/public.h"
#include "camera.h"
#include "clouds/public.h"
#include "terrain/public.h"
#include "textures/public.h"
#include "water/public.h"
#include "renderer.h"
#include "terrain/ter_raster.h"
#include "WaterDefinition.h"

static Scenery _main_scenery;

Scenery::Scenery():
    BaseDefinition(NULL)
{
    atmosphere = (AtmosphereDefinition*)AtmosphereDefinitionClass.create();
    camera = cameraCreateDefinition();
    clouds = (CloudsDefinition*)CloudsDefinitionClass.create();
    terrain = (TerrainDefinition*)TerrainDefinitionClass.create();
    textures = (TexturesDefinition*)TexturesDefinitionClass.create();
    water = new WaterDefinition(this);

    addChild(water);

    _custom_load = NULL;
    _custom_save = NULL;
    _custom_data = NULL;
}

Scenery::~Scenery()
{
    removeChild(water);

    AtmosphereDefinitionClass.destroy(atmosphere);
    cameraDeleteDefinition(camera);
    CloudsDefinitionClass.destroy(clouds);
    TerrainDefinitionClass.destroy(terrain);
    TexturesDefinitionClass.destroy(textures);
    delete water;
}

Scenery* Scenery::getCurrent()
{
    return &_main_scenery;
}

void Scenery::setCustomSaveCallbacks(SceneryCustomDataCallback callback_save, SceneryCustomDataCallback callback_load, void* data)
{
    _custom_save = callback_save;
    _custom_load = callback_load;
    _custom_data = data;
}

void Scenery::save(PackStream* stream) const
{
    BaseDefinition::save(stream);

    noiseSave(stream);

    AtmosphereDefinitionClass.save(stream, atmosphere);
    cameraSave(stream, camera);
    CloudsDefinitionClass.save(stream, clouds);
    TerrainDefinitionClass.save(stream, terrain);
    TexturesDefinitionClass.save(stream, textures);

    if (_custom_save)
    {
        _custom_save(stream, _custom_data);
    }
}

void Scenery::load(PackStream* stream)
{
    BaseDefinition::load(stream);

    noiseLoad(stream);

    AtmosphereDefinitionClass.load(stream, atmosphere);
    cameraLoad(stream, camera);
    CloudsDefinitionClass.load(stream, clouds);
    TerrainDefinitionClass.load(stream, terrain);
    TexturesDefinitionClass.load(stream, textures);

    if (_custom_load)
    {
        _custom_load(stream, _custom_data);
    }
}

void Scenery::autoPreset(int seed)
{
    if (!seed)
    {
        seed = time(NULL);
    }
    srand(seed);

    terrainAutoPreset(terrain, TERRAIN_PRESET_STANDARD);
    texturesAutoPreset(textures, TEXTURES_PRESET_FULL);
    atmosphereAutoPreset(atmosphere, ATMOSPHERE_PRESET_CLEAR_DAY);
    water->applyPreset(WATER_PRESET_LAKE);
    cloudsAutoPreset(clouds, CLOUDS_PRESET_PARTLY_CLOUDY);

    cameraSetLocation(camera, VECTOR_ZERO);
    cameraSetTarget(camera, VECTOR_NORTH);
    cameraValidateDefinition(camera, 1);
}

void Scenery::setAtmosphere(AtmosphereDefinition* atmosphere)
{
    AtmosphereDefinitionClass.copy(atmosphere, this->atmosphere);
}

void Scenery::getAtmosphere(AtmosphereDefinition* atmosphere)
{
    AtmosphereDefinitionClass.copy(this->atmosphere, atmosphere);
}

void Scenery::setCamera(CameraDefinition* camera)
{
    cameraCopyDefinition(camera, this->camera);
}

void Scenery::getCamera(CameraDefinition* camera)
{
    cameraCopyDefinition(this->camera, camera);
}

void Scenery::setClouds(CloudsDefinition* clouds)
{
    CloudsDefinitionClass.copy(clouds, this->clouds);
}

void Scenery::getClouds(CloudsDefinition* clouds)
{
    CloudsDefinitionClass.copy(this->clouds, clouds);
}

void Scenery::setTerrain(TerrainDefinition* terrain)
{
    TerrainDefinitionClass.copy(terrain, this->terrain);
}

void Scenery::getTerrain(TerrainDefinition* terrain)
{
    TerrainDefinitionClass.copy(this->terrain, terrain);
}

void Scenery::setTextures(TexturesDefinition* textures)
{
    TexturesDefinitionClass.copy(textures, this->textures);
}

void Scenery::getTextures(TexturesDefinition* textures)
{
    TexturesDefinitionClass.copy(this->textures, textures);
}

void Scenery::setWater(WaterDefinition* water)
{
    water->copy(this->water);
}

void Scenery::getWater(WaterDefinition* water)
{
    this->water->copy(water);
}

static RayCastingResult _rayWalking(Renderer* renderer, Vector3 location, Vector3 direction, int, int, int, int)
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

static double _getPrecision(Renderer* renderer, Vector3 location)
{
    Vector3 projected;

    projected = cameraProject(renderer->render_camera, location);
    projected.x += 1.0;
    //projected.y += 1.0;

    return v3Norm(v3Sub(cameraUnproject(renderer->render_camera, projected), location)); // / (double)render_quality;
}

void Scenery::bindToRenderer(Renderer* renderer)
{
    // TODO Get rid of this !
    renderer->rayWalking = _rayWalking;
    renderer->getPrecision = _getPrecision;

    cameraCopyDefinition(camera, renderer->render_camera);
    AtmosphereRendererClass.bind(renderer, atmosphere);
    TerrainRendererClass.bind(renderer, terrain);
    TexturesRendererClass.bind(renderer, textures);
    CloudsRendererClass.bind(renderer, clouds);
    WaterRendererClass.bind(renderer, water);
}

// Transitional C-API

void sceneryRenderFirstPass(Renderer* renderer)
{
    terrainRenderSurface(renderer);
    waterRenderSurface(renderer);
    atmosphereRenderSkydome(renderer);
}
