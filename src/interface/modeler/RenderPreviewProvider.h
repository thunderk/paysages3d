#pragma once

#include "modeler_global.h"

#include <QQuickImageProvider>
#include "CanvasLiveClient.h"

namespace paysages {
namespace modeler {

/**
 * Provider for a Qml Image content, filled from a canvas rendering.
 */
class RenderPreviewProvider : public QQuickImageProvider, public CanvasLiveClient {
  public:
    RenderPreviewProvider();
    virtual ~RenderPreviewProvider();

    /**
     * Set the canvas to watch and display, null to stop watching.
     *
     * This function must be called from the graphics thread.
     */
    void setCanvas(const Canvas *canvas);

    /**
     * Release the bound canvas.
     */
    void releaseCanvas();

    /**
     * Call when the image is hidden, to clear it for next render.
     */
    void hide();

    /**
     * Set the tone mapping to apply to pixel colors.
     */
    void setToneMapping(const ColorProfile &profile);

  protected:
    virtual QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;

    virtual void canvasResized(int width, int height) override;
    virtual void canvasCleared(const Color &col) override;
    virtual void canvasPainted(int x, int y, const Color &col) override;

  private:
    QImage *pixbuf;
    const Canvas *canvas;
};
}
}
