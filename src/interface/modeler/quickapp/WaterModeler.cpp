#include "WaterModeler.h"

#include "MainModelerWindow.h"
#include "Scenery.h"
#include "TerrainDefinition.h"
#include "FloatNode.h"
#include "Logs.h"

WaterModeler::WaterModeler(MainModelerWindow *main):
    main(main)
{
    QObject *item = main->findQmlObject("water_level");
    if (item)
    {
        item->setProperty("value", propWaterHeight()->getValue() * 0.5 + 0.5);
        connect(item, SIGNAL(changed(double)), this, SLOT(waterLevelChanged(double)));
    }
}

void WaterModeler::waterLevelChanged(double value)
{
    propWaterHeight()->setValue(value * 2.0 - 1.0);
}

FloatNode *WaterModeler::propWaterHeight() const
{
    return main->getScenery()->getTerrain()->propWaterHeight();
}
