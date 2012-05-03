#ifndef _PAYSAGES_QT_BASEPREVIEW_H_
#define _PAYSAGES_QT_BASEPREVIEW_H_

#include <QMutex>
#include <QImage>
#include <QWidget>
#include <QThread>
#include <QVector>
#include <QList>

class BasePreview : public QWidget {
    Q_OBJECT

public:
    BasePreview(QWidget* parent);
    ~BasePreview();

    static void initDrawers();
    static void stopDrawers();
    
    void redraw();
    
    QImage startChunkTransaction(int x, int y, int w, int h, int* revision);
    void commitChunkTransaction(QImage* chunk, int x, int y, int w, int h, int revision);

    QColor getPixelColor(int x, int y);

protected:
    virtual void updateData();
    virtual QColor getColor(double x, double y);

    void configScaling(double min, double max, double step, double init, bool logarithmic = true);
    void configScrolling(double xmin, double xmax, double xinit, double ymin, double ymax, double yinit);

    double xoffset;
    double yoffset;
    double scaling;

private:
    void updateScaling();
    void updateChunks();
    void invalidatePixbuf(int value);

    void showEvent(QShowEvent* event);
    void resizeEvent(QResizeEvent* event);
    void paintEvent(QPaintEvent* event);

    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);

    QMutex* lock_drawing;
    QImage* pixbuf;

    int _width;
    int _height;
    
    int _revision;

    int mousex;
    int mousey;

    double scalingbase;

    bool alive;

    double conf_scroll_xmin;
    double conf_scroll_xmax;
    double conf_scroll_xinit;
    double conf_scroll_ymin;
    double conf_scroll_ymax;
    double conf_scroll_yinit;

    double conf_scale_min;
    double conf_scale_max;
    double conf_scale_init;
    double conf_scale_step;
    bool conf_scroll_logarithmic;

signals:
    void contentChange();
    void redrawRequested();

private slots:
    void handleRedraw();
};



/*** Private section ***/
class PreviewChunk;

class PreviewDrawingThread : public QThread {
public:
    PreviewDrawingThread();
    void askStop();

    static inline void usleep(int us) {
        QThread::usleep(us);
    }

protected:
    void run();

private:
    bool _running;
};

class PreviewDrawingManager {
public:
    PreviewDrawingManager();
    void startThreads();
    void stopThreads();
    void addChunk(PreviewChunk* chunk);
    void removeChunks(BasePreview* preview);
    void updateChunks(BasePreview* preview);
    void performOneThreadJob();

private:
    int _thread_count;
    QVector<PreviewDrawingThread*> _threads;
    QVector<PreviewChunk*> _chunks;
    QList<PreviewChunk*> _updateQueue;
    QMutex _lock;
};


#endif
