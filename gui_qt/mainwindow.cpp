#include "mainwindow.h"

#include <QApplication>
#include <QMenuBar>
#include <QMenu>
#include <QIcon>
#include <QFileDialog>
#include <QTabWidget>

#include "formatmosphere.h"
#include "formclouds.h"
#include "formlighting.h"
#include "formsky.h"
#include "formterrain.h"
#include "formtextures.h"
#include "formwater.h"
#include "formrender.h"

#include "dialogrender.h"
#include "dialogwanderer.h"

#include "../lib_paysages/main.h"
#include "../lib_paysages/auto.h"
#include "../lib_paysages/scenery.h"

int main(int argc, char** argv)
{
    int result;
    
    paysagesInit();

    QApplication app(argc, argv);
    MainWindow window;

    window.show();

    result = app.exec();
    
    paysagesQuit();
    return result;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    BaseForm* form;
    QTabWidget* tabs;
    QMenu* menu;

    tabs = new QTabWidget(this);

    form = new FormTerrain(tabs);
    tabs->addTab(form, "Terrain");
    QObject::connect(form, SIGNAL(configApplied()), this, SLOT(refreshAll()));

    form = new FormTextures(tabs);
    tabs->addTab(form, "Textures");
    QObject::connect(form, SIGNAL(configApplied()), this, SLOT(refreshAll()));

    form = new FormWater(tabs);
    tabs->addTab(form, "Water");
    QObject::connect(form, SIGNAL(configApplied()), this, SLOT(refreshAll()));

    form = new FormAtmosphere(tabs);
    tabs->addTab(form, "Atmosphere");
    QObject::connect(form, SIGNAL(configApplied()), this, SLOT(refreshAll()));

    form = new FormSky(tabs);
    tabs->addTab(form, "Sky");
    QObject::connect(form, SIGNAL(configApplied()), this, SLOT(refreshAll()));

    form = new FormClouds(tabs);
    tabs->addTab(form, "Clouds");
    QObject::connect(form, SIGNAL(configApplied()), this, SLOT(refreshAll()));

    form = new FormLighting(tabs);
    tabs->addTab(form, "Lighting");
    QObject::connect(form, SIGNAL(configApplied()), this, SLOT(refreshAll()));

    form = new FormRender(tabs);
    tabs->addTab(form, "Render");
    //QObject::connect(form, SIGNAL(configApplied()), this, SLOT(refreshAll()));

    menu = menuBar()->addMenu("Scene");
    menu->addAction("New", this, SLOT(fileNew()), QKeySequence(Qt::CTRL + Qt::Key_N));
    menu->addSeparator();
    menu->addAction("Save", this, SLOT(fileSave()), QKeySequence(Qt::CTRL + Qt::Key_S));
    menu->addAction("Open", this, SLOT(fileLoad()), QKeySequence(Qt::CTRL + Qt::Key_O));
    menu->addSeparator();
    menu->addAction("Quit", this, SLOT(close()), QKeySequence(Qt::CTRL + Qt::Key_Q));

    menu = menuBar()->addMenu("Actions");
    menu->addAction("Explore in 3D", this, SLOT(explore3D()), QKeySequence("F2"));
    menu->addAction("Quick render", this, SLOT(quickPreview()), QKeySequence("F5"));

    setCentralWidget(tabs);

    setWindowTitle("Paysages 3D");
    setWindowIcon(QIcon("images/logo_32.png"));
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
    autoSetDaytime(8, 30);
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

void MainWindow::explore3D()
{
    CameraDefinition camera;

    sceneryGetCamera(&camera);

    DialogWanderer* dialog = new DialogWanderer(this, &camera, true);
    dialog->exec();
    
    scenerySetCamera(&camera);

    delete dialog;
    
    refreshAll();
}
