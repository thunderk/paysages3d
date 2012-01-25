#ifndef _PAYSAGES_QT_PREVIEW_H_
#define _PAYSAGES_QT_PREVIEW_H_

#include <QMutex>
#include <QImage>
#include <QWidget>
#include <QThread>

class Preview:public QWidget
{
    Q_OBJECT

public:
    Preview(QWidget* parent);
    ~Preview();

    void doRender();
    void redraw();

    void setScaling(double scaling);

protected:
    virtual void updateData();
    virtual QColor getColor(double x, double y);

    double xoffset;
    double yoffset;
    double scaling;

private:
    void forceRender();
    void renderPixbuf();
    static void startUpdater();

    void resizeEvent(QResizeEvent* event);
    void paintEvent(QPaintEvent* event);

    QThread* updater;

    QMutex* lock_drawing;
    QImage* pixbuf;

    int mousex;
    int mousey;

    bool alive;
    bool need_rerender;
    bool need_render;

    double conf_scroll_xmin;
    double conf_scroll_xmax;
    double conf_scroll_ymin;
    double conf_scroll_ymax;

    double conf_scale_min;
    double conf_scale_max;
    double conf_scale_step;

signals:
    void contentChange();
    void redrawRequested();

private slots:
    void handleRedraw();
};

#endif
