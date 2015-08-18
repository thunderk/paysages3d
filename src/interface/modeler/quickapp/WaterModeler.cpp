#include "WaterModeler.h"

#include "MainModelerWindow.h"
#include "Scenery.h"
#include "TerrainDefinition.h"
#include "FloatPropertyBind.h"

WaterModeler::WaterModeler(MainModelerWindow *main)
{
    prop_water_height = new FloatPropertyBind(main, "water_height", "value", main->getScenery()->getTerrain()->propWaterHeight());
}

WaterModeler::~WaterModeler()
{
    delete prop_water_height;
}
