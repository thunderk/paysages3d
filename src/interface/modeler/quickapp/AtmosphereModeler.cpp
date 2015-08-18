#include "AtmosphereModeler.h"

#include "MainModelerWindow.h"
#include "Scenery.h"
#include "AtmosphereDefinition.h"
#include "FloatPropertyBind.h"

AtmosphereModeler::AtmosphereModeler(MainModelerWindow *main)
{
    prop_daytime = new FloatPropertyBind(main, "atmosphere_daytime", "value", main->getScenery()->getAtmosphere()->propDayTime());
}

AtmosphereModeler::~AtmosphereModeler()
{
    delete prop_daytime;
}
