#include "AtmosphereModeler.h"

#include "MainModelerWindow.h"
#include "Scenery.h"
#include "AtmosphereDefinition.h"
#include "FloatPropertyBind.h"

AtmosphereModeler::AtmosphereModeler(MainModelerWindow *main)
{
    prop_daytime = new FloatPropertyBind(main, "atmosphere_daytime", "value", main->getScenery()->getAtmosphere()->propDayTime());
    prop_humidity = new FloatPropertyBind(main, "atmosphere_humidity", "value", main->getScenery()->getAtmosphere()->propHumidity());
}

AtmosphereModeler::~AtmosphereModeler()
{
    delete prop_daytime;
    delete prop_humidity;
}
