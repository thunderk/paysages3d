#include "AtmosphereModeler.h"

#include "MainModelerWindow.h"
#include "Scenery.h"
#include "AtmosphereDefinition.h"
#include "DefinitionNode.h"
#include "ModelerCameras.h"

AtmosphereModeler::AtmosphereModeler(MainModelerWindow *ui) : BaseModelerTool(ui) {
    addFloatBinding("atmosphere_humidity", "value", "/atmosphere/humidity");
    addFloatBinding("atmosphere_sun_direction", "phi", "/atmosphere/sun/phi", true);
    addFloatBinding("atmosphere_sun_direction", "theta", "/atmosphere/sun/theta", true);
    addFloatBinding("atmosphere_sun_radius", "value", "/atmosphere/sun/radius");
    addFloatBinding("atmosphere_moon_direction", "phi", "/atmosphere/moon/phi", true);
    addFloatBinding("atmosphere_moon_direction", "theta", "/atmosphere/moon/theta", true);
    // addFloatBinding("atmosphere_moon_radius", "value", "/atmosphere/moon/radius");

    ui->setQmlProperty("atmosphere_daytime", "value", ui->getScenery()->getAtmosphere()->getDaytime());
}

void AtmosphereModeler::nodeChanged(const DefinitionNode *node, const DefinitionDiff *) {
    if (node->getPath().find("/atmosphere/sun/") == 0) {
        getWindow()->getCamera()->startSunTool(1000);
    }
}
