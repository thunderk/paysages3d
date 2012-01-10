#ifndef _PAYSAGES_QT_DIALOGRENDER_H_
#define _PAYSAGES_QT_DIALOGRENDER_H_

#include <QDialog>
#include <QThread>
#include <QProgressBar>

class DialogRender : public QDialog
{
    Q_OBJECT
public:
    explicit DialogRender(QWidget *parent, int quality, int width, int height);
    QImage* pixbuf;
    QWidget* area;
    QProgressBar* progress;
    int progress_value;

public slots:
    virtual void reject();

protected:
    virtual void closeEvent(QCloseEvent* e);

private:
    QThread* render_thread;
};

#endif // _PAYSAGES_QT_DIALOGRENDER_H_
