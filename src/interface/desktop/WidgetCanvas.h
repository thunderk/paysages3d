#ifndef WIDGETCANVAS_H
#define WIDGETCANVAS_H

#include "desktop_global.h"

#include <QWidget>

namespace paysages {
namespace desktop {

/*!
 * \brief Widget to display the full content of a Canvas.
 */
class WidgetCanvas : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetCanvas(QWidget *parent = 0);

signals:

public slots:

};

}
}

#endif // WIDGETCANVAS_H
