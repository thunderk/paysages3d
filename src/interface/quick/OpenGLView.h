#ifndef OPENGLVIEW_H
#define OPENGLVIEW_H

#include "quick_global.h"

#include <QQuickItem>

namespace paysages {
namespace quick {

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
