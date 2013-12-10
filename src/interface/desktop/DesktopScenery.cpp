#include "DesktopScenery.h"

#include "common/mainwindow.h"

static DesktopScenery _main_scenery;

void DesktopScenery::save(PackStream *stream) const
{
    Scenery::save(stream);

    MainWindow::instance()->guiSave(stream);
}

void DesktopScenery::load(PackStream *stream)
{
    Scenery::load(stream);

    MainWindow::instance()->guiLoad(stream);
}

DesktopScenery* DesktopScenery::getCurrent()
{
    return &_main_scenery;
}
