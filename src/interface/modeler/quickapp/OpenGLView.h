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

protected:
    virtual void hoverMoveEvent(QHoverEvent *event) override;
    virtual void timerEvent(QTimerEvent *event) override;

private:
    bool initialized;
    QQuickWindow *window;
};

}
}

#endif // OPENGLVIEW_H
