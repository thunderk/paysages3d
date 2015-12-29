#include "AtmosphereModeler.h"

#include "MainModelerWindow.h"
#include "Scenery.h"
#include "AtmosphereDefinition.h"

AtmosphereModeler::AtmosphereModeler(MainModelerWindow *main) : BaseModelerTool(main) {
    addFloatBinding("atmosphere_humidity", "value", "/atmosphere/humidity");
    addFloatBinding("atmosphere_sun_direction", "phi", "/atmosphere/sun/phi");
    addFloatBinding("atmosphere_sun_direction", "theta", "/atmosphere/sun/theta");
    addFloatBinding("atmosphere_sun_radius", "value", "/atmosphere/sun/radius");
    addFloatBinding("atmosphere_moon_direction", "phi", "/atmosphere/moon/phi");
    addFloatBinding("atmosphere_moon_direction", "theta", "/atmosphere/moon/theta");
    // addFloatBinding("atmosphere_moon_radius", "value", "/atmosphere/moon/radius");

    main->setQmlProperty("atmosphere_daytime", "value", main->getScenery()->getAtmosphere()->getDaytime());
}
