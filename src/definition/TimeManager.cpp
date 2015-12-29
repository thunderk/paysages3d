#include "TimeManager.h"

#include "Scenery.h"
#include "AtmosphereDefinition.h"
#include "FloatNode.h"
#include "CloudsDefinition.h"
#include "CloudLayerDefinition.h"
#include "WaterDefinition.h"

TimeManager::TimeManager() {
    wind_x = 0.0;
    wind_z = 0.0;
}

void TimeManager::moveForward(Scenery *scenery, double amount) {
    // Move the sun
    scenery->getAtmosphere()->setDayTime(scenery->getAtmosphere()->getDaytime() + amount);

    // Move the clouds
    int n = scenery->getClouds()->getLayerCount();
    for (int i = 0; i < n; i++) {
        CloudLayerDefinition *cloud = scenery->getClouds()->getCloudLayer(i);
        cloud->propXOffset()->addValue(-wind_x * amount * 100.0);
        cloud->propZOffset()->addValue(-wind_z * amount * 100.0);
        // TODO Alter noise offsets
    }

    // Move the water
    WaterDefinition *water = scenery->getWater();
    water->propXOffset()->addValue(-wind_x * amount * 10.0);
    water->propZOffset()->addValue(-wind_z * amount * 10.0);
    // TODO Alter noise offsets
}

void TimeManager::setWind(double wind_x, double wind_z) {
    this->wind_x = wind_x;
    this->wind_z = wind_z;
}
