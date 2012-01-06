#ifndef _PAYSAGES_QT_DIALOGRENDER_H_
#define _PAYSAGES_QT_DIALOGRENDER_H_

#include <QDialog>
#include <QThread>
#include <QProgressBar>

class DialogRender : public QDialog
{
    Q_OBJECT
public:
    explicit DialogRender(QWidget *parent = 0);
    QImage* pixbuf;
    QWidget* area;
    QProgressBar* progress;
    int progress_value;

signals:

public slots:

private:
    QThread* render_thread;
};

#endif // _PAYSAGES_QT_DIALOGRENDER_H_
