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

    QObject *findQmlObject(const QString &objectName);
    void setQmlProperty(const QString &objectName, const QString &propertyName, const QVariant &value);

    QString getState() const;
    void setState(const QString &stateName);

    inline Scenery *getScenery() const {return scenery;}
    inline OpenGLRenderer *getRenderer() const {return renderer;}
    inline ModelerCameras *getCamera() const {return cameras;}

protected:
    virtual void keyReleaseEvent(QKeyEvent *event) override;

private:
    OpenGLRenderer *renderer;
    Scenery *scenery;

    AtmosphereModeler *atmosphere;
    WaterModeler *water;

    ModelerCameras *cameras;

    RenderPreviewProvider *render_preview_provider;
    RenderProcess *render_process;
};

}
}

#endif // MAINMODELERWINDOW_H
