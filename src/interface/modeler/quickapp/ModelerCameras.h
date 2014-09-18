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
