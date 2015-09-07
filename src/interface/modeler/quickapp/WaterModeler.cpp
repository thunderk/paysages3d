#include "WaterModeler.h"

#include "MainModelerWindow.h"
#include "Scenery.h"
#include "TerrainDefinition.h"
#include "WaterDefinition.h"
#include "FloatPropertyBind.h"
#include "OpenGLRenderer.h"
#include "OpenGLWater.h"

WaterModeler::WaterModeler(MainModelerWindow *ui):
    ui(ui)
{
    QObject *toggle_water = ui->findQmlObject("camera_toggle_water");
    if (toggle_water)
    {
        connect(toggle_water, SIGNAL(toggled(bool)), this, SLOT(enableRendering(bool)));
    }

    prop_water_height = new FloatPropertyBind(ui, "water_height", "value", ui->getScenery()->getTerrain()->propWaterHeight());
    prop_water_reflexion = new FloatPropertyBind(ui, "water_reflection", "value", ui->getScenery()->getWater()->propReflection());
}

WaterModeler::~WaterModeler()
{
    delete prop_water_height;
    delete prop_water_reflexion;
}

void WaterModeler::enableRendering(bool enable)
{
    ui->getRenderer()->getWater()->setEnabled(enable);
}
