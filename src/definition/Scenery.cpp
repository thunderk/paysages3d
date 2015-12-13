#include "Scenery.h"

#include <map>

#include "PackStream.h"
#include "AtmosphereDefinition.h"
#include "CameraDefinition.h"
#include "CloudsDefinition.h"
#include "TerrainDefinition.h"
#include "TexturesDefinition.h"
#include "VegetationDefinition.h"
#include "WaterDefinition.h"
#include "Logs.h"
#include "RandomGenerator.h"

static const double APP_HEADER = 19866544632.125;
static const int DATA_VERSION = 1;

Scenery::Scenery() : DefinitionNode(NULL, "scenery", "scenery") {
    atmosphere = new AtmosphereDefinition(this);
    camera = new CameraDefinition(this);
    clouds = new CloudsDefinition(this);
    terrain = new TerrainDefinition(this);
    textures = new TexturesDefinition(this);
    water = new WaterDefinition(this);
    vegetation = new VegetationDefinition(this);
}

void Scenery::validate() {
    DefinitionNode::validate();

    keepCameraAboveGround(camera);
}

Scenery::FileOperationResult Scenery::saveGlobal(const string &filepath) const {
    PackStream stream;
    double app_header = (double)APP_HEADER;
    double version_header = (double)DATA_VERSION;

    if (not stream.bindToFile(filepath, true)) {
        return FILE_OPERATION_IOERROR;
    }

    stream.write(&app_header);
    stream.write(&version_header);

    save(&stream);

    stream.write(&version_header);
    stream.write(&app_header);

    Logs::debug() << "[Definition] Scenery saved to file: " << filepath << endl;
    return FILE_OPERATION_OK;
}

Scenery::FileOperationResult Scenery::loadGlobal(const string &filepath) {
    PackStream stream;
    double app_header, version_header;

    if (not stream.bindToFile(filepath, false)) {
        return FILE_OPERATION_IOERROR;
    }

    stream.read(&app_header);
    if (app_header != APP_HEADER) {
        return FILE_OPERATION_APP_MISMATCH;
    }

    stream.read(&version_header);
    if ((int)version_header != DATA_VERSION) {
        return FILE_OPERATION_VERSION_MISMATCH;
    }

    load(&stream);

    // Reset read headers, to test them again
    app_header = -1;
    version_header = -1;

    stream.read(&version_header);
    if ((int)version_header != DATA_VERSION) {
        return FILE_OPERATION_VERSION_MISMATCH;
    }

    stream.read(&app_header);
    if (app_header != APP_HEADER) {
        return FILE_OPERATION_APP_MISMATCH;
    }

    Logs::debug() << "[Definition] Scenery loaded from file: " << filepath << endl;
    return FILE_OPERATION_OK;
}

const Scenery *Scenery::getScenery() const {
    return this;
}

void Scenery::autoPreset(RandomGenerator &random) {
    terrain->applyPreset(TerrainDefinition::TERRAIN_PRESET_STANDARD, random);
    textures->applyPreset(TexturesDefinition::TEXTURES_PRESET_FULL, random);
    atmosphere->applyPreset(AtmosphereDefinition::ATMOSPHERE_PRESET_CLEAR_DAY, random);
    water->applyPreset(WaterDefinition::WATER_PRESET_LAKE, random);
    clouds->applyPreset(CloudsDefinition::CLOUDS_PRESET_PARTLY_CLOUDY, random);
    vegetation->applyPreset(VegetationDefinition::VEGETATION_PRESET_TEMPERATE, random);

    camera->setLocation(VECTOR_ZERO);
    camera->setTarget(VECTOR_NORTH);

    validate();

    Logs::debug() << "[Definition] New scenery generated from seed " << random.getSeed() << endl;
}

void Scenery::autoPreset(unsigned int seed) {
    RandomGenerator random(seed);
    autoPreset(random);
}

void Scenery::setAtmosphere(AtmosphereDefinition *atmosphere) {
    atmosphere->copy(this->atmosphere);
}

void Scenery::getAtmosphere(AtmosphereDefinition *atmosphere) {
    this->atmosphere->copy(atmosphere);
}

void Scenery::setCamera(CameraDefinition *camera) {
    camera->copy(this->camera);
    keepCameraAboveGround(this->camera);
}

void Scenery::getCamera(CameraDefinition *camera) {
    this->camera->copy(camera);
}

void Scenery::setClouds(CloudsDefinition *clouds) {
    clouds->copy(this->clouds);
}

void Scenery::getClouds(CloudsDefinition *clouds) {
    this->clouds->copy(clouds);
}

void Scenery::setTerrain(TerrainDefinition *terrain) {
    terrain->copy(this->terrain);
}

void Scenery::getTerrain(TerrainDefinition *terrain) {
    this->terrain->copy(terrain);
}

void Scenery::setTextures(TexturesDefinition *textures) {
    textures->copy(this->textures);
}

void Scenery::getTextures(TexturesDefinition *textures) {
    this->textures->copy(textures);
}

void Scenery::setVegetation(VegetationDefinition *vegetation) {
    vegetation->copy(this->vegetation);
}

void Scenery::getVegetation(VegetationDefinition *vegetation) {
    this->vegetation->copy(vegetation);
}

void Scenery::setWater(WaterDefinition *water) {
    water->copy(this->water);
}

void Scenery::getWater(WaterDefinition *water) {
    this->water->copy(water);
}

void Scenery::keepCameraAboveGround(CameraDefinition *camera) {
    Vector3 camera_location = camera->getLocation();
    double terrain_height = terrain->getInterpolatedHeight(camera_location.x, camera_location.z, true, true) + 1.0;
    double water_height = 0.5;
    if (camera_location.y < water_height || camera_location.y < terrain_height) {
        double diff = ((water_height > terrain_height) ? water_height : terrain_height) - camera_location.y;
        camera->setLocation(camera_location.add(Vector3(0.0, diff, 0.0)));
    }
}
