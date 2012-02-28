#include "formrender.h"

#include <QFileDialog>
#include <QMessageBox>
#include "dialogrender.h"
#include "inputcamera.h"
#include "../lib_paysages/render.h"
#include "../lib_paysages/scenery.h"

/**************** Form ****************/
FormRender::FormRender(QWidget *parent) :
    BaseForm(parent, true)
{
    QPushButton* button;

    _quality = 5;
    _width = 800;
    _height = 600;
    _camera = cameraCreateDefinition();

    addInput(new InputCamera(this, tr("Camera"), &_camera));
    addInputInt(tr("Quality"), &_quality, 1, 10, 1, 1);
    addInputInt(tr("Image width"), &_width, 100, 2000, 10, 100);
    addInputInt(tr("Image height"), &_height, 100, 2000, 10, 100);

    button = addButton(tr("Start new render"));
    connect(button, SIGNAL(clicked()), this, SLOT(startRender()));
    button = addButton(tr("Show last render"));
    connect(button, SIGNAL(clicked()), this, SLOT(showRender()));
    button = addButton(tr("Save last render"));
    connect(button, SIGNAL(clicked()), this, SLOT(saveRender()));

    revertConfig();
}

void FormRender::revertConfig()
{
    sceneryGetCamera(&_camera);
    BaseForm::revertConfig();
}

void FormRender::applyConfig()
{
    scenerySetCamera(&_camera);
    BaseForm::applyConfig();
}

void FormRender::configChangeEvent()
{
    cameraValidateDefinition(&_camera, 1);
    BaseForm::configChangeEvent();
}

void FormRender::startRender()
{
    DialogRender* dialog = new DialogRender(this);
    dialog->startRender(_quality, _width, _height);

    delete dialog;
}

void FormRender::showRender()
{
    DialogRender* dialog = new DialogRender(this);
    dialog->loadLastRender();

    delete dialog;
}

void FormRender::saveRender()
{
    QString filepath;

    filepath = QFileDialog::getSaveFileName(this, tr("Choose a filename to save the last render"));
    if (!filepath.isNull())
    {
        //renderSaveToFile((char*)filepath.toStdString().c_str());
        QMessageBox::information(this, "Message", QString(tr("The picture %1 has been saved.")).arg(filepath));
    }
}
