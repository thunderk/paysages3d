#include <QApplication>
#include "mainwindow.h"
#include "formwater.h"
#include "formsky.h"
#include "formrender.h"

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
    tabs->addTab(new BaseForm(tabs), "Temp");
    tabs->addTab(new FormWater(tabs), "Water");
    tabs->addTab(new FormSky(tabs), "Sky");
    tabs->addTab(new FormRender(tabs), "Render");

    setCentralWidget(tabs);
}
