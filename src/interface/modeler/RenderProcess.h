#pragma once

#include "modeler_global.h"

#include <QObject>

namespace paysages {
namespace modeler {

class RenderProcess : public QObject {
    Q_OBJECT

  public:
    RenderProcess(MainModelerWindow *window, RenderPreviewProvider *destination);
    virtual ~RenderProcess();

    /**
     * Start the rendering process in a separate thread.
     */
    void startRender(Scenery *scenery, const RenderConfig &config);

    /**
     * Get the size of the preview image.
     */
    const QSize getPreviewSize();

    inline bool isRendering() const {
        return rendering;
    }

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

    /**
     * Stop any currently running render.
     */
    void stopRender();

    /**
     * Save the current picture to a given path on disk.
     */
    void savePicture(QString path);

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
