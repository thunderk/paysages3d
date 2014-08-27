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

private:
    bool initialized;
};

}
}

#endif // OPENGLVIEW_H
