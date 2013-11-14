#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QMenuBar>
#include <QMenu>
#include <QIcon>
#include <QFileDialog>
#include <QTabWidget>
#include <QTranslator>
#include <QLocale>
#include <QMessageBox>
#include <QSplashScreen>

#include "BasePreview.h"
#include "PreviewOsd.h"
#include "PreviewOsdItem.h"
#include "CameraDefinition.h"
#include "formclouds.h"
#include "formatmosphere.h"
#include "formwater.h"
#include "formrender.h"

#include "terrain/mainterrainform.h"

#include "dialogrender.h"
#include "dialogexplorer.h"

#include "main.h"
#include "Scenery.h"
#include "PackStream.h"
#include "tools.h"

MainWindow* MainWindow::_instance = NULL;

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

    if (myTranslator.load("paysages_" + QLocale::system().name(), "./data/i18n") || myTranslator.load("paysages_" + QLocale::system().name(), "/usr/share/paysages3d/i18n"))
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
    //window->showMaximized();
    splash->finish(window);

    delete splash;

    result = app.exec();

    BasePreview::stopDrawers();

    delete window;

    paysagesQuit();
    return result;
}

MainWindow::MainWindow() :
    QMainWindow(),
    ui(new Ui::MainWindow)
{
    _instance = this;
    ui->setupUi(this);

    BaseForm* form;

    connect(ui->action_explore, SIGNAL(triggered()), this, SLOT(explore3D()));
    connect(ui->action_quick_render, SIGNAL(triggered()), this, SLOT(quickPreview()));
    connect(ui->action_final_render, SIGNAL(triggered()), this, SLOT(finalRender()));
    connect(ui->action_last_render, SIGNAL(triggered()), this, SLOT(showLastRender()));
    connect(ui->action_file_new, SIGNAL(triggered()), this, SLOT(fileNew()));
    connect(ui->action_file_save, SIGNAL(triggered()), this, SLOT(fileSave()));
    connect(ui->action_file_load, SIGNAL(triggered()), this, SLOT(fileLoad()));
    connect(ui->action_about, SIGNAL(triggered()), this, SLOT(showAboutDialog()));

    form = new FormWater(ui->tabs);
    ui->tabs->addTab(form, QIcon(getDataPath("images/tab_water.png")), tr("Water"));
    QObject::connect(form, SIGNAL(configApplied()), this, SLOT(refreshAll()), Qt::QueuedConnection);
    _forms.append(form);

    form = new FormAtmosphere(ui->tabs);
    ui->tabs->addTab(form, QIcon(getDataPath("images/tab_atmosphere.png")), tr("Atmosphere"));
    QObject::connect(form, SIGNAL(configApplied()), this, SLOT(refreshAll()), Qt::QueuedConnection);
    _forms.append(form);

    form = new FormClouds(ui->tabs);
    ui->tabs->addTab(form, QIcon(getDataPath("images/tab_clouds.png")), tr("Clouds"));
    QObject::connect(form, SIGNAL(configApplied()), this, SLOT(refreshAll()), Qt::QueuedConnection);
    _forms.append(form);

    _form_render = new FormRender(ui->tabs);
    ui->tabs->addTab(_form_render, QIcon(getDataPath("images/tab_render.png")), tr("Render"));
    _forms.append(_form_render);

    ui->tabs->setCurrentIndex(0);

    // TODO Decide this according to platform / screen size
    ui->toolBar->hide();
    ui->tool_panel->hide();
    //ui->menuBar->hide();

    Scenery::getCurrent()->setCustomSaveCallbacks(MainWindow::guiSaveCallback, MainWindow::guiLoadCallback, this);

    // FIXME AutoPreset has already been called by paysagesInit but we need to redo it here because
    //   the auto apply on FormRender overwrites the camera. Delete this when the render form is no longer a BaseForm.
    Scenery::getCurrent()->autoPreset(0);
    refreshAll();
}

MainWindow::~MainWindow()
{
    delete ui;
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
    PreviewOsd* osd = PreviewOsd::getInstance(QString("geolocation"));
    osd->clearItems();
    PreviewOsdItem* item = osd->newItem(50, 50);
    item->drawCamera(Scenery::getCurrent()->getCamera());
    item->setToolTip(QString(tr("Camera")));

    emit refreshed();
}

void MainWindow::openTab(int position)
{
    ui->tabs->setCurrentIndex(position);
}

void MainWindow::fileNew()
{
    if (QMessageBox::question(this, tr("Paysages 3D - New scenery"), tr("Do you want to start a new scenery ? Any unsaved changes will be lost."), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
    {
        Scenery::getCurrent()->autoPreset(0);
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

void MainWindow::finalRender()
{
    _form_render->startRender();
}

void MainWindow::showLastRender()
{
    _form_render->showRender();
}

void MainWindow::explore3D()
{
    CameraDefinition* camera = new CameraDefinition;
    int result;

    Scenery::getCurrent()->getCamera(camera);

    DialogExplorer* dialog = new DialogExplorer(this, camera, true);
    result = dialog->exec();

    delete dialog;

    if (result == QDialog::Accepted)
    {
        Scenery::getCurrent()->setCamera(camera);
        refreshAll();
    }

    delete camera;
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
