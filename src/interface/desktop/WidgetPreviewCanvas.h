#ifndef WIDGETPREVIEWCANVAS_H
#define WIDGETPREVIEWCANVAS_H

#include "desktop_global.h"

#include <QWidget>
#include "CanvasLiveClient.h"

namespace paysages {
namespace desktop {

/*!
 * \brief Widget to display a live-updated preview of a Canvas software rendering.
 */
class WidgetPreviewCanvas : public QWidget, public CanvasLiveClient
{
    Q_OBJECT
public:
    explicit WidgetPreviewCanvas(QWidget *parent = 0);

    /*!
     * \brief Set the canvas to watch and display, null to stop watching.
     */
    void setCanvas(const Canvas *canvas);

protected:
    virtual void canvasResized(int width, int height);
    virtual void canvasCleared(const Color &col);
    virtual void canvasPainted(int x, int y, const Color &col);
    virtual void timerEvent(QTimerEvent *event);

private:
    const Canvas *canvas;
};

}
}

#endif // WIDGETPREVIEWCANVAS_H
