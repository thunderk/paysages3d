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
    RenderProcess(MainModelerWindow *window, RenderPreviewProvider *destination);
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

    inline bool isRendering() const {return rendering;}

public slots:
    /**
     * Start a quick render.
     */
    void startQuickRender();

    /**
     * Start a medium render.
     */
    void startMediumRender();

    /**
     * Start a final render.
     */
    void startFinalRender();

    /**
     * Show the previous rendering.
     */
    void showPreviousRender();

protected:
    virtual void timerEvent(QTimerEvent *event) override;

private:
    MainModelerWindow *window;
    RenderPreviewProvider *destination;
    bool has_render;
    bool rendering;
    SoftwareCanvasRenderer *renderer;
    Thread *render_thread;
};

}
}

#endif // RENDERPROCESS_H
