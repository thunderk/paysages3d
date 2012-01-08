#include "mainwindow.h"

#include <QApplication>
#include <QMenuBar>
#include <QFileDialog>

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
    QMenu* menu;

    tabs = new QTabWidget(this);
    tabs->addTab(new BaseForm(tabs), "Temp");
    tabs->addTab(new FormWater(tabs), "Water");
    tabs->addTab(new FormSky(tabs), "Sky");
    tabs->addTab(new FormRender(tabs), "Render");

    menu = menuBar()->addMenu("File");
    menu->addAction("New", this, SLOT(fileNew()));
    menu->addAction("Save", this, SLOT(fileSave()));
    menu->addAction("Load", this, SLOT(fileLoad()));
    menu->addAction("Quit", this, SLOT(close()));

    setCentralWidget(tabs);
}

void MainWindow::fileNew()
{
    // TODO
}

void MainWindow::fileSave()
{
    QString filepath = QFileDialog::getSaveFileName(this);
    autoSave((char*)filepath.toStdString().c_str());
}

void MainWindow::fileLoad()
{
    QString filepath = QFileDialog::getOpenFileName(this);
    autoLoad((char*)filepath.toStdString().c_str());
}
