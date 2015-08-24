#ifndef OPENGLVIEW_H
#define OPENGLVIEW_H

#include "modeler_global.h"

#include <QQuickItem>

namespace paysages {
namespace modeler {

class OpenGLView : public QQuickItem
{
    Q_OBJECT
public:
    explicit OpenGLView(QQuickItem *parent = 0);

public slots:
    void handleWindowChanged(QQuickWindow *win);
    void paint();
    void handleResize();

protected:
    virtual void wheelEvent(QWheelEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void timerEvent(QTimerEvent *event) override;

private:
    bool acceptInputs() const;
    double getSpeedFactor(QInputEvent *event);

private:
    bool initialized;
    bool resized;
    MainModelerWindow *window;
    OpenGLRenderer *renderer;

    Qt::MouseButton mouse_button;
    QPointF mouse_pos;
};

}
}

#endif // OPENGLVIEW_H
