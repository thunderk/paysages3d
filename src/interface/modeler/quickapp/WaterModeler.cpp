#include "WaterModeler.h"

#include "MainModelerWindow.h"

WaterModeler::WaterModeler(MainModelerWindow *main):
    main(main)
{
    QObject *item = main->findQmlObject("water_level");
    if (item)
    {
        connect(item, SIGNAL(changed(double)), this, SLOT(waterLevelChanged(double)));
    }
}

void WaterModeler::waterLevelChanged(double value)
{
    // TODO
    //qDebug() << "water level : " << value;
}
