#ifndef MAINMODELERWINDOW_H
#define MAINMODELERWINDOW_H

#include "modeler_global.h"

#include <QQuickView>

namespace paysages {
namespace modeler {

class MainModelerWindow: public QQuickView
{
    Q_OBJECT
public:
    MainModelerWindow();
    virtual ~MainModelerWindow();

    inline OpenGLRenderer *getRenderer() const {return renderer;}

private:
    OpenGLRenderer *renderer;
    Scenery *scenery;
};

}
}

#endif // MAINMODELERWINDOW_H
