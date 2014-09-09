#ifndef RENDERPROCESS_H
#define RENDERPROCESS_H

#include "modeler_global.h"

#include <QObject>

namespace paysages {
namespace modeler {

class RenderProcess: public QObject
{
    Q_OBJECT

public:
    RenderProcess(RenderPreviewProvider *destination);
    virtual ~RenderProcess();

    /**
     * Start the rendering process in a separate thread.
     */
    void startRender(Scenery *scenery, const RenderConfig &config);

    /**
     * Stop any currently running render.
     */
    void stopRender();

    /**
     * Get the size of the preview image.
     */
    const QSize getPreviewSize();

protected:
    virtual void timerEvent(QTimerEvent *event) override;

private:
    RenderPreviewProvider *destination;
    bool rendering;
    SoftwareCanvasRenderer *renderer;
    Thread *render_thread;
};

}
}

#endif // RENDERPROCESS_H
