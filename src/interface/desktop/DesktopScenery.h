#ifndef DESKTOPSCENERY_H
#define DESKTOPSCENERY_H

#include "desktop_global.h"

#include "Scenery.h"

class DesktopScenery: public Scenery
{
public:
    virtual void save(PackStream* stream) const override;
    virtual void load(PackStream* stream) override;

    static DesktopScenery* getCurrent();
};

#endif // DESKTOPSCENERY_H
