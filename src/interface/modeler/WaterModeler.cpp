#include "WaterModeler.h"

#include "MainModelerWindow.h"
#include "Scenery.h"
#include "TerrainDefinition.h"
#include "WaterDefinition.h"
#include "FloatPropertyBind.h"
#include "IntPropertyBind.h"
#include "OpenGLRenderer.h"
#include "OpenGLWater.h"

WaterModeler::WaterModeler(MainModelerWindow *ui) : BaseModelerTool(ui) {
    QObject *toggle_water = ui->findQmlObject("camera_toggle_water");
    if (toggle_water) {
        connect(toggle_water, SIGNAL(toggled(bool)), this, SLOT(enableRendering(bool)));
    }

    addIntBinding("water_model", "value", "/water/model");
    addFloatBinding("water_height", "value", "/terrain/water_height");
    addFloatBinding("water_reflection", "value", "/water/reflection");
}

void WaterModeler::enableRendering(bool enable) {
    getWindow()->getRenderer()->getWater()->setEnabled(enable);
}
