#ifndef _PAYSAGES_QT_BASEPREVIEW_H_
#define _PAYSAGES_QT_BASEPREVIEW_H_

#include <QMutex>
#include <QImage>
#include <QWidget>
#include <QThread>
#include <QVector>
#include <QList>
#include <QLabel>
#include <QHash>
#include "previewosd.h"
#include "common/previewrenderer.h"
#include "rendering/tools/pack.h"
#include "rendering/tools/color.h"

class _ContextChoice
{
public:
    QString title;
    QStringList items;
    int current;
};

class _ContextToggle
{
public:
    QString title;
    bool value;
};

class BasePreview : public QWidget
{
    Q_OBJECT

public:
    BasePreview(QWidget* parent);
    ~BasePreview();

    void setRenderer(PreviewRenderer* renderer);

    virtual void savePack(PackStream* stream);
    virtual void loadPack(PackStream* stream);

    static void initDrawers();
    static void stopDrawers();
    static void reviveAll();

    void redraw();

    QImage startChunkTransaction(int x, int y, int w, int h, int* revision);
    bool commitChunkTransaction(QImage* chunk, int x, int y, int w, int h, int revision);
    void rollbackChunkTransaction();

    QColor getPixelColor(int x, int y);

    void addOsd(QString name);

    void configHdrToneMapping(bool active);
    void configScaling(double min, double max, double step, double init, bool logarithmic = true);
    void configScrolling(double xmin, double xmax, double xinit, double ymin, double ymax, double yinit);

    void addChoice(const QString& key, const QString& title, const QStringList& choices, int init_value);
    virtual void choiceChangeEvent(const QString& key, int position);

    void addToggle(const QString& key, const QString& text, bool init_value);
    virtual void toggleChangeEvent(QString key, bool value);

protected:
    virtual void updateData();
    virtual void cameraEvent();
    virtual Color getColor(double x, double y);

    double xoffset;
    double yoffset;
    double scaling;

private:
    void updateScaling();
    void updateChunks();
    void invalidatePixbuf(int value);

    void timerEvent(QTimerEvent* event);
    void showEvent(QShowEvent* event);
    void resizeEvent(QResizeEvent* event);
    void paintEvent(QPaintEvent* event);

    void contextMenuEvent(QContextMenuEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);
    void leaveEvent(QEvent* event);

    QMutex* _lock_drawing;
    QImage* _pixbuf;
    QVector<PreviewOsd*> _osd;
    QHash<QString, _ContextChoice> _choices;
    QHash<QString, _ContextToggle> _toggles;

    QLabel* _info;

    PreviewRenderer* _renderer;

    int _width;
    int _height;

    int _revision;
    int _transactions_count;
    bool _redraw_requested;

    int mousex;
    int mousey;

    double scalingbase;

    bool alive;
    bool _hdr_enabled;
    ColorProfile* _hdr_profile;

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
    void choiceSelected(QAction* action);
};



/*** Private section ***/
class PreviewChunk;

class PreviewDrawingThread : public QThread
{
public:
    PreviewDrawingThread();
    void askStop();

    static inline void usleep(int us)
    {
        QThread::usleep(us);
    }

protected:
    void run();

private:
    bool _running;
};

class PreviewDrawingManager
{
public:
    PreviewDrawingManager();
    void startThreads();
    void stopThreads();
    void addChunk(PreviewChunk* chunk);
    void removeChunks(BasePreview* preview);
    void updateChunks(BasePreview* preview);
    void suspendChunks(BasePreview* preview);
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
