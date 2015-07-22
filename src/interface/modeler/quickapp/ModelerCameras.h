#ifndef MODELERCAMERAS_H
#define MODELERCAMERAS_H

#include "modeler_global.h"

#include <QObject>

namespace paysages {
namespace modeler {

/**
 * Storage for modeler cameras.
 */
class ModelerCameras: public QObject
{
    Q_OBJECT

public:
    ModelerCameras(MainModelerWindow *parent);
    ~ModelerCameras();

    /**
     * Process a zoom request.
     */
    void processZoom(double value);

    /**
     * Process a scroll request.
     */
    void processScroll(double xvalue, double yvalue);

protected:
    void timerEvent(QTimerEvent *event);

public slots:
    void changeActiveCamera(const QString &name);

private:
    MainModelerWindow *parent;
    CameraDefinition *active;
    CameraDefinition *current;
    CameraDefinition *render;
    CameraDefinition *topdown;
};

}
}

#endif // MODELERCAMERAS_H
