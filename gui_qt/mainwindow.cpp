#include <QApplication>
#include "mainwindow.h"
#include "formwater.h"

#include "../lib_paysages/shared/functions.h"

int main(int argc, char** argv)
{
    paysagesInit();

    QApplication app(argc, argv);
    MainWindow window;

    window.show();

    Preview::startUpdater();

    return app.exec();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    QTabWidget* tabs;

    tabs = new QTabWidget(this);
    tabs->addTab(new FormWater(tabs), "Water");

    setCentralWidget(tabs);
}
