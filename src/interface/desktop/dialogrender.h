#ifndef _PAYSAGES_QT_DIALOGRENDER_H_
#define _PAYSAGES_QT_DIALOGRENDER_H_

#include "desktop_global.h"

#include <ctime>
#include <QDialog>

class QThread;
class QProgressBar;
class QSlider;
class QComboBox;
class QLabel;
class QScrollArea;
class QMutex;

class DialogRender : public QDialog
{
    Q_OBJECT
public:
    explicit DialogRender(QWidget *parent, SoftwareCanvasRenderer* renderer);
    ~DialogRender();

    void tellRenderSize(int width, int height);
    void tellProgressChange(double value);
    void tellRenderEnded();
    void startRender(const RenderConfig &params);
    void loadLastRender();

    QImage* pixbuf;
    QMutex* pixbuf_lock;
    QWidget* area;

private slots:
    void applyRenderSize(int width, int height);
    void applyProgress(double value);
    void saveRender();
    void applyRenderEnded();
    void toneMappingChanged();

signals:
    void renderSizeChanged(int width, int height);
    void progressChanged(double value);
    void renderEnded();

private:
    SoftwareCanvasRenderer* canvas_renderer;
    WidgetPreviewCanvas* canvas_preview;

    QScrollArea* _scroll;
    QWidget* _info;
    QWidget* _actions;
    QComboBox* _tonemapping_control;
    QSlider* _exposure_control;
    QPushButton* _save_button;
    QThread* _render_thread;
    QLabel* _timer;
    QProgressBar* _progress;
    time_t _started;
};

#endif
