#ifndef MODELERCAMERAS_H
#define MODELERCAMERAS_H

#include "modeler_global.h"

#include <QObject>
#include "DefinitionWatcher.h"

namespace paysages {
namespace modeler {

/**
 * Storage for modeler cameras.
 */
class ModelerCameras: public QObject, public DefinitionWatcher
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

    /**
     * Process a panning rotation.
     */
    void processPanning(double xvalue, double yvalue);

    /**
     * Start a sun tool, the camera will follow the sun.
     */
    void startSunTool();

    /**
     * End the tool mode.
     */
    void endTool();

protected:
    void timerEvent(QTimerEvent *event);

    virtual void nodeChanged(const DefinitionNode *node, const DefinitionDiff *diff) override;

    /**
     * Validate current camera, pushing it to rendered scenery if needed.
     */
    void validate();

public slots:
    void changeActiveCamera(const QString &name);
    void toolChanged(const QString &tool);

private:
    MainModelerWindow *parent;
    CameraDefinition *active;
    CameraDefinition *current;
    CameraDefinition *render;
    CameraDefinition *topdown;
    CameraDefinition *tool;

    typedef enum
    {
        TOOL_NONE,
        TOOL_SUN
    } CameraToolMode;
    CameraToolMode tool_mode;
};

}
}

#endif // MODELERCAMERAS_H
