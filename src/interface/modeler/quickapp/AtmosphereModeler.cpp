#include "AtmosphereModeler.h"

#include "MainModelerWindow.h"
#include "Scenery.h"
#include "AtmosphereDefinition.h"
#include "OpenGLRenderer.h"
#include "OpenGLSkybox.h"

AtmosphereModeler::AtmosphereModeler(MainModelerWindow *main):
    main(main)
{
    QObject *item = main->findQmlObject("atmosphere_daytime");
    if (item)
    {
        connect(item, SIGNAL(changed(double)), this, SLOT(daytimeChanged(double)));
    }
}

void AtmosphereModeler::daytimeChanged(double value)
{
    main->getScenery()->getAtmosphere()->setDaytime(value);

    main->getRenderer()->getScenery()->setAtmosphere(main->getScenery()->getAtmosphere());

    main->getRenderer()->getSkybox()->update();
}
