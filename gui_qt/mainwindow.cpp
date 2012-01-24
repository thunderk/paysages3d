#include "mainwindow.h"

#include <QApplication>
#include <QMenuBar>
#include <QMenu>
#include <QFileDialog>
#include <QTabWidget>

#include "formterrain.h"
#include "formwater.h"
#include "formsky.h"
#include "formrender.h"
#include "dialogrender.h"

#include "../lib_paysages/auto.h"
#include "../lib_paysages/shared/functions.h"

int main(int argc, char** argv)
{
    paysagesInit();

    QApplication app(argc, argv);
    MainWindow window;

    window.show();

    return app.exec();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    QTabWidget* tabs;
    QMenu* menu;

    tabs = new QTabWidget(this);
    tabs->addTab(new FormTerrain(tabs), "Terrain");
    tabs->addTab(new FormWater(tabs), "Water");
    tabs->addTab(new FormSky(tabs), "Sky");
    tabs->addTab(new FormRender(tabs), "Render");

    menu = menuBar()->addMenu("Scene");
    menu->addAction("New", this, SLOT(fileNew()));
    menu->addAction("Save", this, SLOT(fileSave()));
    menu->addAction("Load", this, SLOT(fileLoad()));
    menu->addAction("Quit", this, SLOT(close()));

    menu = menuBar()->addMenu("Render");
    menu->addAction("Quick render", this, SLOT(quickPreview()), QKeySequence("F2"));

    setCentralWidget(tabs);

    setWindowTitle("Paysages 3D");
}

void MainWindow::refreshAll()
{
    QList<BaseForm*> list_forms = this->findChildren<BaseForm*>("_base_form_");
    for (int i = 0; i < list_forms.size(); i++)
    {
        list_forms[i]->revertConfig();
    }
}

void MainWindow::fileNew()
{
    autoGenRealisticLandscape(0);
    refreshAll();
}

void MainWindow::fileSave()
{
    QString filepath = QFileDialog::getSaveFileName(this);
    if (!filepath.isNull())
    {
        paysagesSave((char*)filepath.toStdString().c_str());
    }
}

void MainWindow::fileLoad()
{
    QString filepath = QFileDialog::getOpenFileName(this);
    if (!filepath.isNull())
    {
        paysagesLoad((char*)filepath.toStdString().c_str());
        refreshAll();
    }
}

void MainWindow::quickPreview()
{
    DialogRender* dialog = new DialogRender(this);
    dialog->startRender(3, 400, 300);

    delete dialog;
}
