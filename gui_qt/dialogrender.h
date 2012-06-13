#ifndef _PAYSAGES_QT_DIALOGRENDER_H_
#define _PAYSAGES_QT_DIALOGRENDER_H_

#include <QDialog>
#include <QThread>
#include <QProgressBar>
#include <QScrollArea>
#include "../lib_paysages/renderer.h"

class DialogRender : public QDialog
{
    Q_OBJECT
public:
    explicit DialogRender(QWidget *parent, Renderer* renderer);
    ~DialogRender();

    void tellRenderSize(int width, int height);
    void startRender(RenderParams params);
    void loadLastRender();

    QImage* pixbuf;
    QWidget* area;
    QProgressBar* progress;
    int progress_value;
    
private slots:
    void applyRenderSize(int width, int height);
    
signals:
    void renderSizeChanged(int width, int height);

private:
    QScrollArea* scroll;
    QThread* render_thread;
    Renderer* _renderer;
};

#endif // _PAYSAGES_QT_DIALOGRENDER_H_
