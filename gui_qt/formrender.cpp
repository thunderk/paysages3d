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

    addInput(new InputCamera(this, "Camera", &_camera));
    addInputInt("Quality", &_quality, 1, 10, 1, 1);
    addInputInt("Image width", &_width, 100, 2000, 10, 100);
    addInputInt("Image height", &_height, 100, 2000, 10, 100);

    button = addButton("Start new render");
    connect(button, SIGNAL(clicked()), this, SLOT(startRender()));
    button = addButton("Show last render");
    connect(button, SIGNAL(clicked()), this, SLOT(showRender()));
    button = addButton("Save last render");
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

    filepath = QFileDialog::getSaveFileName(this, "Choose a filename to save the last render");
    if (!filepath.isNull())
    {
        renderSaveToFile((char*)filepath.toStdString().c_str());
        QMessageBox::information(this, "Message", "The picture " + filepath + " has been saved.");
    }
}
