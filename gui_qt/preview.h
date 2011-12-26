#ifndef PREVIEW_H
#define PREVIEW_H

#include <QMutex>
#include <QImage>
#include <QWidget>

class Preview:public QWidget
{
public:
    Preview(QWidget* parent);
    static void startUpdater();
    void doRender();

protected:
    void resizeEvent(QResizeEvent* event);
    void paintEvent(QPaintEvent* event);
    virtual QColor getColor(double x, double y) = 0;

private:
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

    int need_rerender;
    int need_render;
    //SmallPreviewCallback renderer;
};

#endif // PREVIEW_H
