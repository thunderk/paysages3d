#include "Scenery.h"

#include <ctime>

#include "NoiseGenerator.h"
#include "PackStream.h"
#include "atmosphere/public.h"
#include "CameraDefinition.h"
#include "CloudsDefinition.h"
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
    camera = new CameraDefinition;
    clouds = new CloudsDefinition(this);
    terrain = (TerrainDefinition*)TerrainDefinitionClass.create();
    textures = (TexturesDefinition*)TexturesDefinitionClass.create();
    water = new WaterDefinition(this);

    addChild(camera);
    addChild(water);
    addChild(clouds);

    _custom_load = NULL;
    _custom_save = NULL;
    _custom_data = NULL;
}

Scenery::~Scenery()
{
    AtmosphereDefinitionClass.destroy(atmosphere);
    TerrainDefinitionClass.destroy(terrain);
    TexturesDefinitionClass.destroy(textures);
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
    TerrainDefinitionClass.load(stream, terrain);
    TexturesDefinitionClass.load(stream, textures);

    if (_custom_load)
    {
        _custom_load(stream, _custom_data);
    }

    validate();
}

void Scenery::validate()
{
    BaseDefinition::validate();

    checkCameraAboveGround();
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
    clouds->applyPreset(CloudsDefinition::CLOUDS_PRESET_PARTLY_CLOUDY);

    camera->setLocation(VECTOR_ZERO);
    camera->setTarget(VECTOR_NORTH);

    validate();
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
    camera->copy(this->camera);
    checkCameraAboveGround();
}

void Scenery::getCamera(CameraDefinition* camera)
{
    this->camera->copy(camera);
}

void Scenery::setClouds(CloudsDefinition* clouds)
{
    clouds->copy(this->clouds);
}

void Scenery::getClouds(CloudsDefinition* clouds)
{
    this->clouds->copy(clouds);
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




#include "clouds/public.h"

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

    projected = renderer->render_camera->project(location);
    projected.x += 1.0;
    //projected.y += 1.0;

    return v3Norm(v3Sub(renderer->render_camera->unproject(projected), location)); // / (double)render_quality;
}

void Scenery::bindToRenderer(Renderer* renderer)
{
    // TODO Get rid of this !
    renderer->rayWalking = _rayWalking;
    renderer->getPrecision = _getPrecision;

    camera->copy(renderer->render_camera);
    AtmosphereRendererClass.bind(renderer, atmosphere);
    TerrainRendererClass.bind(renderer, terrain);
    TexturesRendererClass.bind(renderer, textures);
    CloudsRendererClass.bind(renderer, clouds);
    WaterRendererClass.bind(renderer, water);
}

void Scenery::checkCameraAboveGround()
{
    Vector3 camera_location = camera->getLocation();
    double terrain_height = terrainGetInterpolatedHeight(terrain, camera_location.x, camera_location.z, 1, 1) + 0.5;
    double water_height = terrainGetWaterHeight(terrain) + 0.5;
    if (camera_location.y < water_height || camera_location.y < terrain_height)
    {
        double diff = ((water_height > terrain_height) ? water_height : terrain_height) - camera_location.y;
        camera->setLocation(camera_location.add(Vector3(0.0, diff, 0.0)));
    }
}

// Transitional C-API

void sceneryRenderFirstPass(Renderer* renderer)
{
    terrainRenderSurface(renderer);
    waterRenderSurface(renderer);
    atmosphereRenderSkydome(renderer);
}
