#include "mainwindow.h"

#include <QApplication>
#include <QMenuBar>
#include <QMenu>
#include <QIcon>
#include <QToolBar>
#include <QFileDialog>
#include <QTabWidget>
#include <QTranslator>
#include <QLocale>
#include <QMessageBox>

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
    MainWindow* window;
    int result;
    
    paysagesInit();

    QApplication app(argc, argv);
    
    QTranslator qtTranslator;
    QTranslator myTranslator;
    
    if (myTranslator.load("paysages_" + QLocale::system().name(), "./i18n"))
    {
        app.installTranslator(&myTranslator);

        qtTranslator.load("qt_" + QLocale::system().name());
        app.installTranslator(&qtTranslator);
    }
    else
    {
        qtTranslator.load("qt_en");
        app.installTranslator(&qtTranslator);
    }

    window = new MainWindow();
    window->show();

    result = app.exec();
    
    delete window;
    
    paysagesQuit();
    return result;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    BaseForm* form;
    QTabWidget* tabs;
    QToolBar* toolbar;
    
    tabs = new QTabWidget(this);

    form = new FormTerrain(tabs);
    tabs->addTab(form, tr("Terrain"));
    QObject::connect(form, SIGNAL(configApplied()), this, SLOT(refreshAll()));

    form = new FormTextures(tabs);
    tabs->addTab(form, tr("Textures"));
    QObject::connect(form, SIGNAL(configApplied()), this, SLOT(refreshAll()));

    form = new FormWater(tabs);
    tabs->addTab(form, tr("Water"));
    QObject::connect(form, SIGNAL(configApplied()), this, SLOT(refreshAll()));

    form = new FormSky(tabs);
    tabs->addTab(form, tr("Sky"));
    QObject::connect(form, SIGNAL(configApplied()), this, SLOT(refreshAll()));

    form = new FormAtmosphere(tabs);
    tabs->addTab(form, tr("Atmosphere"));
    QObject::connect(form, SIGNAL(configApplied()), this, SLOT(refreshAll()));

    form = new FormClouds(tabs);
    tabs->addTab(form, tr("Clouds"));
    QObject::connect(form, SIGNAL(configApplied()), this, SLOT(refreshAll()));

    /*form = new FormLighting(tabs);
    tabs->addTab(form, tr("Lighting"));
    QObject::connect(form, SIGNAL(configApplied()), this, SLOT(refreshAll()));*/

    _form_render = new FormRender(tabs);
    tabs->addTab(_form_render, tr("Render"));

    toolbar = new QToolBar(this);
    toolbar->setOrientation(Qt::Vertical);
    toolbar->setAllowedAreas(Qt::LeftToolBarArea);
    toolbar->setMovable(false);
    toolbar->setFloatable(false);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolbar->toggleViewAction()->setEnabled(false);
    toolbar->setIconSize(QSize(32, 32));
    addToolBar(Qt::LeftToolBarArea, toolbar);
    
    toolbar->addAction(QIcon("images/new.png"), tr("&New"), this, SLOT(fileNew()))->setShortcut(QKeySequence(tr("Crtl+N")));
    toolbar->addAction(QIcon("images/save.png"), tr("&Save"), this, SLOT(fileSave()))->setShortcut(QKeySequence(tr("Crtl+S")));
    toolbar->addAction(QIcon("images/load.png"), tr("&Load"), this, SLOT(fileLoad()))->setShortcut(QKeySequence(tr("Crtl+L")));
    toolbar->addAction(QIcon("images/explore.png"), tr("&Explore (F2)"), this, SLOT(explore3D()))->setShortcut(QKeySequence(tr("F2")));
    toolbar->addAction(QIcon("images/render.png"), tr("&Quick\nrender (F5)"), this, SLOT(quickPreview()))->setShortcut(QKeySequence(tr("F5")));
    toolbar->addAction(QIcon("images/about.png"), tr("&About"), this, SLOT(showAboutDialog()));

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

void MainWindow::showAboutDialog()
{
    QMessageBox::about(this, tr("Paysages 3D"), tr("A 3D landscape editing and rendering software.\n\nCredits :\nProgramming - Michael Lemaire"));
}

void MainWindow::quickPreview()
{
    _form_render->startQuickRender();
}

void MainWindow::explore3D()
{
    CameraDefinition camera;
    int result;

    sceneryGetCamera(&camera);

    DialogWanderer* dialog = new DialogWanderer(this, &camera, true);
    result = dialog->exec();
    
    delete dialog;
    
    if (result == QDialog::Accepted)
    {
        scenerySetCamera(&camera);
        refreshAll();
    }
}
