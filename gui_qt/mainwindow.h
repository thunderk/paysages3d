#ifndef _PAYSAGES_QT_MAINWINDOW_H_
#define _PAYSAGES_QT_MAINWINDOW_H_

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
};

#endif // _PAYSAGES_QT_MAINWINDOW_H_
