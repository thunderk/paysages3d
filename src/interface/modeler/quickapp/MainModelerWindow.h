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
    void connectQmlSignal(const QString &objectName, const char *signal, const QObject *receiver, const char *method);

    QString getState() const;
    void setState(const QString &stateName);

    inline Scenery *getScenery() const {return scenery;}
    inline OpenGLRenderer *getRenderer() const {return renderer;}
    inline ModelerCameras *getCamera() const {return cameras;}

public slots:
    void newFile();
    void saveFile();
    void loadFile();
    void exit();

protected:
    virtual void keyReleaseEvent(QKeyEvent *event) override;

private:
    Scenery *scenery;
    OpenGLRenderer *renderer;

    AtmosphereModeler *atmosphere;
    WaterModeler *water;

    ModelerCameras *cameras;

    RenderPreviewProvider *render_preview_provider;
    RenderProcess *render_process;
};

}
}

#endif // MAINMODELERWINDOW_H
