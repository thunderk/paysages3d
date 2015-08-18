#include "AtmosphereModeler.h"

#include "MainModelerWindow.h"
#include "Scenery.h"
#include "AtmosphereDefinition.h"
#include "OpenGLRenderer.h"
#include "OpenGLSkybox.h"
#include "OpenGLTerrain.h"
#include "FloatNode.h"

AtmosphereModeler::AtmosphereModeler(MainModelerWindow *main):
    main(main)
{
    QObject *item = main->findQmlObject("atmosphere_daytime");
    if (item)
    {
        item->setProperty("value", propDayTime()->getValue());
        connect(item, SIGNAL(changed(double)), this, SLOT(daytimeChanged(double)));
    }
}

void AtmosphereModeler::daytimeChanged(double value)
{
    propDayTime()->setValue(value);
}

FloatNode *AtmosphereModeler::propDayTime() const
{
    return main->getScenery()->getAtmosphere()->propDayTime();
}
