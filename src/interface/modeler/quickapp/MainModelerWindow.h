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

    QObject *findQmlObject(const QString& objectName);

    inline Scenery *getScenery() const {return scenery;}
    inline OpenGLRenderer *getRenderer() const {return renderer;}

protected:
    virtual void keyReleaseEvent(QKeyEvent *event) override;

private:
    OpenGLRenderer *renderer;
    Scenery *scenery;

    AtmosphereModeler *atmosphere;
    WaterModeler *water;

    RenderPreviewProvider *render_preview_provider;
    RenderProcess *render_process;
};

}
}

#endif // MAINMODELERWINDOW_H
