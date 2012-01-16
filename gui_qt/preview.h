#ifndef _PAYSAGES_QT_PREVIEW_H_
#define _PAYSAGES_QT_PREVIEW_H_

#include <QMutex>
#include <QImage>
#include <QWidget>

class Preview:public QWidget
{
    Q_OBJECT

public:
    Preview(QWidget* parent);
    ~Preview();

    static void startUpdater();
    void doRender();
    void redraw();

protected:
    void resizeEvent(QResizeEvent* event);
    void paintEvent(QPaintEvent* event);
    virtual QColor getColor(double x, double y) = 0;

    void renderPixbuf();
    void forceRender();

    double conf_scroll_xmin;
    double conf_scroll_xmax;
    double conf_scroll_ymin;
    double conf_scroll_ymax;

    double conf_scale_min;
    double conf_scale_max;
    double conf_scale_step;

    double xoffset;
    double yoffset;
    double scaling;

    QMutex* lock;
    QImage* pixbuf;

    int mousex;
    int mousey;

    bool alive;
    bool need_rerender;
    bool need_render;
};

#endif
