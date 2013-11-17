#include "Scenery.h"

#include <ctime>

#include "NoiseGenerator.h"
#include "PackStream.h"
#include "AtmosphereDefinition.h"
#include "CameraDefinition.h"
#include "CloudsDefinition.h"
#include "TerrainDefinition.h"
#include "TexturesDefinition.h"
#include "WaterDefinition.h"

Scenery::Scenery():
    BaseDefinition(NULL)
{
    addChild(atmosphere = new AtmosphereDefinition(this));
    addChild(camera = new CameraDefinition);
    addChild(clouds = new CloudsDefinition(this));
    addChild(terrain = new TerrainDefinition(this));
    addChild(textures = new TexturesDefinition(this));
    addChild(water = new WaterDefinition(this));
}

void Scenery::save(PackStream* stream) const
{
    BaseDefinition::save(stream);

    noiseSave(stream);
}

void Scenery::load(PackStream* stream)
{
    BaseDefinition::load(stream);

    noiseLoad(stream);

    validate();
}

void Scenery::validate()
{
    BaseDefinition::validate();

    checkCameraAboveGround();
}

Scenery* Scenery::getScenery()
{
    return this;
}

void Scenery::autoPreset(int seed)
{
    if (!seed)
    {
        seed = time(NULL);
    }
    srand(seed);

    terrain->applyPreset(TerrainDefinition::TERRAIN_PRESET_STANDARD);
    textures->applyPreset(TexturesDefinition::TEXTURES_PRESET_FULL);
    atmosphere->applyPreset(AtmosphereDefinition::ATMOSPHERE_PRESET_CLEAR_DAY);
    water->applyPreset(WaterDefinition::WATER_PRESET_LAKE);
    clouds->applyPreset(CloudsDefinition::CLOUDS_PRESET_PARTLY_CLOUDY);

    camera->setLocation(VECTOR_ZERO);
    camera->setTarget(VECTOR_NORTH);

    validate();
}

void Scenery::setAtmosphere(AtmosphereDefinition* atmosphere)
{
    atmosphere->copy(this->atmosphere);
}

void Scenery::getAtmosphere(AtmosphereDefinition* atmosphere)
{
    this->atmosphere->copy(atmosphere);
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
    terrain->copy(this->terrain);
}

void Scenery::getTerrain(TerrainDefinition* terrain)
{
    this->terrain->copy(terrain);
}

void Scenery::setTextures(TexturesDefinition* textures)
{
    textures->copy(this->textures);
}

void Scenery::getTextures(TexturesDefinition* textures)
{
    this->textures->copy(textures);
}

void Scenery::setWater(WaterDefinition* water)
{
    water->copy(this->water);
}

void Scenery::getWater(WaterDefinition* water)
{
    this->water->copy(water);
}

void Scenery::checkCameraAboveGround()
{
    Vector3 camera_location = camera->getLocation();
    double terrain_height = terrain->getInterpolatedHeight(camera_location.x, camera_location.z, 1, 1) + 0.5;
    double water_height = terrain->getWaterHeight() + 0.5;
    if (camera_location.y < water_height || camera_location.y < terrain_height)
    {
        double diff = ((water_height > terrain_height) ? water_height : terrain_height) - camera_location.y;
        camera->setLocation(camera_location.add(Vector3(0.0, diff, 0.0)));
    }
}