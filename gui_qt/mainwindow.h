#ifndef _PAYSAGES_QT_MAINWINDOW_H_
#define _PAYSAGES_QT_MAINWINDOW_H_

#include <QMainWindow>
#include "formrender.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

public slots:
    void refreshAll();

    void fileNew();
    void fileSave();
    void fileLoad();

    void quickPreview();
    void explore3D();
    
private:
    FormRender* _form_render;
};

#endif
