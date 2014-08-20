#ifndef WIDGETPREVIEWCANVAS_H
#define WIDGETPREVIEWCANVAS_H

#include "desktop_global.h"

#include <QWidget>
#include "CanvasLiveClient.h"

namespace paysages {
namespace desktop {

/**
 * Widget to display a live-updated preview of a Canvas software rendering.
 */
class WidgetPreviewCanvas : public QWidget, public CanvasLiveClient
{
    Q_OBJECT
public:
    explicit WidgetPreviewCanvas(QWidget *parent = 0);
    virtual ~WidgetPreviewCanvas();

    /**
     * Set the canvas to watch and display, null to stop watching.
     *
     * This function must be called from the graphics thread.
     */
    void setCanvas(const Canvas *canvas);

    /**
     * Set the tone mapping to apply to pixel colors.
     */
    void setToneMapping(const ColorProfile &profile);

    virtual void paintEvent(QPaintEvent* event);

protected:
    virtual void canvasResized(int width, int height);
    virtual void canvasCleared(const Color &col);
    virtual void canvasPainted(int x, int y, const Color &col);
    virtual void timerEvent(QTimerEvent *event);

private:
    QImage* pixbuf;
    const Canvas *canvas;
};

}
}

#endif // WIDGETPREVIEWCANVAS_H
