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
#include <QSplashScreen>

#include "basepreview.h"
#include "formclouds.h"
#include "formatmosphere.h"
#include "formtextures.h"
#include "formwater.h"
#include "formrender.h"

#include "terrain/mainterrainform.h"

#include "dialogrender.h"
#include "dialogexplorer.h"

#include "rendering/main.h"
#include "rendering/auto.h"
#include "rendering/scenery.h"
#include "tools.h"

int main(int argc, char** argv)
{
    MainWindow* window;
    QSplashScreen* splash;
    int result;

    QApplication app(argc, argv);

    splash = new QSplashScreen(QPixmap(getDataPath("images/logo_256.png")));
    splash->show();

    QTranslator qtTranslator;
    QTranslator myTranslator;

    if (myTranslator.load("paysages_" + QLocale::system().name(), "./data/i18n"))
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

    //splash->showMessage(app.tr("Preloading..."), Qt::AlignCenter, Qt::white);
    app.processEvents();

    paysagesInit();
    BasePreview::initDrawers();

    window = new MainWindow();
    window->show();
    splash->finish(window);

    delete splash;

    result = app.exec();

    BasePreview::stopDrawers();

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
    tabs->setIconSize(QSize(32, 32));

    tabs->addTab(new MainTerrainForm(tabs), QIcon(getDataPath("images/tab_terrain.png")), tr("Landscape shape"));

    form = new FormTextures(tabs);
    tabs->addTab(form, QIcon(getDataPath("images/tab_textures.png")), tr("Textures"));
    QObject::connect(form, SIGNAL(configApplied()), this, SLOT(refreshAll()), Qt::QueuedConnection);
    _forms.append(form);

    form = new FormWater(tabs);
    tabs->addTab(form, QIcon(getDataPath("images/tab_water.png")), tr("Water"));
    QObject::connect(form, SIGNAL(configApplied()), this, SLOT(refreshAll()), Qt::QueuedConnection);
    _forms.append(form);

    form = new FormAtmosphere(tabs);
    tabs->addTab(form, QIcon(getDataPath("images/tab_atmosphere.png")), tr("Atmosphere"));
    QObject::connect(form, SIGNAL(configApplied()), this, SLOT(refreshAll()), Qt::QueuedConnection);
    _forms.append(form);

    form = new FormClouds(tabs);
    tabs->addTab(form, QIcon(getDataPath("images/tab_clouds.png")), tr("Clouds"));
    QObject::connect(form, SIGNAL(configApplied()), this, SLOT(refreshAll()), Qt::QueuedConnection);
    _forms.append(form);

    _form_render = new FormRender(tabs);
    tabs->addTab(_form_render, QIcon(getDataPath("images/tab_render.png")), tr("Render"));
    _forms.append(_form_render);

    toolbar = new QToolBar(this);
    toolbar->setOrientation(Qt::Vertical);
    toolbar->setAllowedAreas(Qt::LeftToolBarArea);
    toolbar->setMovable(false);
    toolbar->setFloatable(false);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolbar->toggleViewAction()->setEnabled(false);
    toolbar->setIconSize(QSize(32, 32));
    addToolBar(Qt::LeftToolBarArea, toolbar);

    toolbar->addAction(QIcon(getDataPath("images/new.png")), tr("&New"), this, SLOT(fileNew()))->setShortcut(QKeySequence(tr("Crtl+N")));
    toolbar->addAction(QIcon(getDataPath("images/save.png")), tr("&Save"), this, SLOT(fileSave()))->setShortcut(QKeySequence(tr("Crtl+S")));
    toolbar->addAction(QIcon(getDataPath("images/load.png")), tr("&Load"), this, SLOT(fileLoad()))->setShortcut(QKeySequence(tr("Crtl+L")));
    toolbar->addAction(QIcon(getDataPath("images/explore.png")), tr("&Explore (F2)"), this, SLOT(explore3D()))->setShortcut(QKeySequence(tr("F2")));
    toolbar->addAction(QIcon(getDataPath("images/render.png")), tr("&Quick\nrender (F5)"), this, SLOT(quickPreview()))->setShortcut(QKeySequence(tr("F5")));
    toolbar->addAction(QIcon(getDataPath("images/about.png")), tr("&About"), this, SLOT(showAboutDialog()));

    setCentralWidget(tabs);

    setWindowTitle("Paysages 3D");
    setWindowIcon(QIcon(getDataPath("images/logo_32.png")));

    scenerySetCustomDataCallback(MainWindow::guiSaveCallback, MainWindow::guiLoadCallback, this);

    refreshAll();
}

bool MainWindow::event(QEvent* event)
{
    if (event->type() == QEvent::WindowActivate)
    {
        BasePreview::reviveAll();
    }

    return QMainWindow::event(event);
}

void MainWindow::refreshAll()
{
    logDebug("[MainWindow] Refreshing whole UI");

    // Refresh all tabs
    for (int i = 0; i < _forms.size(); i++)
    {
        _forms[i]->revertConfig();
    }

    // Refresh preview OSD
    CameraDefinition* camera = cameraCreateDefinition();
    PreviewOsd* osd = PreviewOsd::getInstance(QString("geolocation"));
    osd->clearItems();
    sceneryGetCamera(camera);
    PreviewOsdItem* item = osd->newItem(50, 50);
    item->drawCamera(camera);
    item->setToolTip(QString(tr("Camera")));
    cameraDeleteDefinition(camera);
}

void MainWindow::fileNew()
{
    if (QMessageBox::question(this, tr("Paysages 3D - New scenery"), tr("Do you want to start a new scenery ? Any unsaved changes will be lost."), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
    {
        autoGenRealisticLandscape(0);
        refreshAll();
    }
}

void MainWindow::fileSave()
{
    QString filepath = QFileDialog::getSaveFileName(this, tr("Paysages 3D - Choose a file to save the scenery"), QString(), tr("Paysages 3D Scenery (*.p3d)"));
    if (!filepath.isNull())
    {
        if (!filepath.toLower().endsWith(".p3d"))
        {
            filepath = filepath.append(".p3d");
        }

        FileOperationResult result = paysagesSave((char*) filepath.toStdString().c_str());
        switch (result)
        {
        case FILE_OPERATION_OK:
            break;
        case FILE_OPERATION_IOERROR:
            QMessageBox::critical(this, tr("Paysages 3D - File saving error"), tr("Can't write specified file : %1").arg(filepath));
            break;
        default:
            QMessageBox::critical(this, tr("Paysages 3D - File saving error"), tr("Unexpected error while saving file : %1").arg(filepath));
        }
    }
}

void MainWindow::fileLoad()
{
    if (QMessageBox::question(this, tr("Paysages 3D - Load scenery"), tr("Do you want to load a scenery from file ? Any unsaved changes will be lost."), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
    {
        QString filepath = QFileDialog::getOpenFileName(this, tr("Paysages 3D - Choose a scenery file to load"), QString(), tr("Paysages 3D Scenery (*.p3d)"));
        if (!filepath.isNull())
        {
            FileOperationResult result = paysagesLoad((char*) filepath.toStdString().c_str());
            switch (result)
            {
            case FILE_OPERATION_OK:
                refreshAll();
                break;
            case FILE_OPERATION_IOERROR:
                QMessageBox::critical(this, tr("Paysages 3D - File loading error"), tr("Can't read specified file : %1").arg(filepath));
                break;
            case FILE_OPERATION_APP_MISMATCH:
                QMessageBox::critical(this, tr("Paysages 3D - File loading error"), tr("This file doesn't look like a Paysages 3D file : %1").arg(filepath));
                break;
            case FILE_OPERATION_VERSION_MISMATCH:
                QMessageBox::critical(this, tr("Paysages 3D - File loading error"), tr("This file was created with an incompatible Paysages 3D version : %1").arg(filepath));
                break;
            default:
                QMessageBox::critical(this, tr("Paysages 3D - File loading error"), tr("Unexpected error while loading file : %1").arg(filepath));
            }
        }
    }
}

void MainWindow::showAboutDialog()
{
    QMessageBox::about(this, tr("Paysages 3D"), tr("A 3D landscape editing and rendering software.\n\nAuthors :\nProgramming - Michael Lemaire\n\nCredits :\nQt - http://qt-project.org/\nDevIL - http://openil.sourceforge.net/\nGLib - http://www.gtk.org/\n"));
}

void MainWindow::quickPreview()
{
    _form_render->startQuickRender();
}

void MainWindow::explore3D()
{
    CameraDefinition* camera;
    int result;

    camera = cameraCreateDefinition();
    sceneryGetCamera(camera);

    DialogExplorer* dialog = new DialogExplorer(this, camera, true);
    result = dialog->exec();

    delete dialog;

    if (result == QDialog::Accepted)
    {
        scenerySetCamera(camera);
        refreshAll();
    }

    cameraDeleteDefinition(camera);
}

void MainWindow::guiSaveCallback(PackStream* stream, void* data)
{
    ((MainWindow*) data)->guiSave(stream);
}

void MainWindow::guiLoadCallback(PackStream* stream, void* data)
{
    ((MainWindow*) data)->guiLoad(stream);
}

void MainWindow::guiSave(PackStream* stream)
{
    // Save all tabs status
    for (int i = 0; i < _forms.size(); i++)
    {
        _forms[i]->savePack(stream);
    }
}

void MainWindow::guiLoad(PackStream* stream)
{
    // Load all tabs status
    for (int i = 0; i < _forms.size(); i++)
    {
        _forms[i]->loadPack(stream);
    }
}
