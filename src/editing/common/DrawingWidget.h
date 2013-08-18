#ifndef DRAWINGWIDGET_H
#define	DRAWINGWIDGET_H

#include <QWidget>

class QPaintEvent;
class QPainter;

class DrawingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DrawingWidget(QWidget* parent = 0);
protected:
    virtual void doDrawing(QPainter* painter) = 0;
private:
    virtual void paintEvent(QPaintEvent* event);
};

#endif	/* DRAWINGWIDGET_H */

