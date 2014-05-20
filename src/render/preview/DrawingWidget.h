#ifndef DRAWINGWIDGET_H
#define	DRAWINGWIDGET_H

#include "preview_global.h"

#include <QWidget>

class QPaintEvent;
class QPainter;

namespace paysages {
namespace preview {

class PREVIEWSHARED_EXPORT DrawingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DrawingWidget(QWidget* parent = 0);
protected:
    virtual void doDrawing(QPainter* painter) = 0;
private:
    virtual void paintEvent(QPaintEvent* event);
};

}
}

#endif	/* DRAWINGWIDGET_H */

