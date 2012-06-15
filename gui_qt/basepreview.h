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
    static void reviveAll();
    
    void redraw();
    
    QImage startChunkTransaction(int x, int y, int w, int h, int* revision);
    void commitChunkTransaction(QImage* chunk, int x, int y, int w, int h, int revision);

    QColor getPixelColor(int x, int y);

protected:
    virtual void updateData();
    virtual QColor getColor(float x, float y);

    void configScaling(float min, float max, float step, float init, bool logarithmic = true);
    void configScrolling(float xmin, float xmax, float xinit, float ymin, float ymax, float yinit);

    float xoffset;
    float yoffset;
    float scaling;

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

    float scalingbase;

    bool alive;

    float conf_scroll_xmin;
    float conf_scroll_xmax;
    float conf_scroll_xinit;
    float conf_scroll_ymin;
    float conf_scroll_ymax;
    float conf_scroll_yinit;

    float conf_scale_min;
    float conf_scale_max;
    float conf_scale_init;
    float conf_scale_step;
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
    void updateAllChunks();
    void performOneThreadJob();
    int chunkCount();

private:
    int _thread_count;
    QVector<PreviewDrawingThread*> _threads;
    QVector<PreviewChunk*> _chunks;
    QList<PreviewChunk*> _updateQueue;
    QMutex _lock;
};


#endif
