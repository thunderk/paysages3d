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
    explicit DialogRender(QWidget *parent);
    ~DialogRender();

    void startRender(int quality, int width, int height);
    void loadLastRender();

    QImage* pixbuf;
    QWidget* area;
    QScrollArea* scroll;
    QProgressBar* progress;
    int progress_value;

private:
    QThread* render_thread;
};

#endif // _PAYSAGES_QT_DIALOGRENDER_H_
