#include "WaterModeler.h"

#include "MainModelerWindow.h"
#include "Scenery.h"
#include "TerrainDefinition.h"
#include "FloatPropertyBind.h"
#include "OpenGLRenderer.h"
#include "OpenGLWater.h"

WaterModeler::WaterModeler(MainModelerWindow *ui):
    ui(ui)
{
    QObject *toggle_water = ui->findQmlObject("camera_toggle_water");
    if (toggle_water)
    {
        connect(toggle_water, SIGNAL(changed(bool)), this, SLOT(enableRendering(bool)));
    }

    prop_water_height = new FloatPropertyBind(ui, "water_height", "value", ui->getScenery()->getTerrain()->propWaterHeight());
}

WaterModeler::~WaterModeler()
{
    delete prop_water_height;
}

void WaterModeler::enableRendering(bool enable)
{
    ui->getRenderer()->getWater()->setEnabled(enable);
}
