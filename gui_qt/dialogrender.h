#ifndef _PAYSAGES_QT_DIALOGRENDER_H_
#define _PAYSAGES_QT_DIALOGRENDER_H_

#include <time.h>
#include <QDialog>
#include <QThread>
#include <QProgressBar>
#include <QScrollArea>
#include <QLabel>
#include "../lib_paysages/renderer.h"

class DialogRender : public QDialog
{
    Q_OBJECT
public:
    explicit DialogRender(QWidget *parent, Renderer* renderer);
    ~DialogRender();

    void tellRenderSize(int width, int height);
    void tellProgressChange(double value);
    void startRender(RenderParams params);
    void loadLastRender();

    QImage* pixbuf;
    QWidget* area;
    
private slots:
    void applyRenderSize(int width, int height);
    void applyProgress(double value);
    
signals:
    void renderSizeChanged(int width, int height);
    void progressChanged(double value);

private:
    QScrollArea* _scroll;
    QWidget* _info;
    QThread* _render_thread;
    QLabel* _timer;
    Renderer* _renderer;
    QProgressBar* _progress;
    time_t _started;
};

#endif // _PAYSAGES_QT_DIALOGRENDER_H_
