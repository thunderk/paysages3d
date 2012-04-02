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
    
    _renderer_inited = false;

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

FormRender::~FormRender()
{
    if (_renderer_inited)
    {
        rendererDelete(&_renderer);
    }
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

void FormRender::startQuickRender()
{
    if (_renderer_inited)
    {
        rendererDelete(&_renderer);
    }
    _renderer = sceneryCreateStandardRenderer();
    _renderer_inited = true;
    
    DialogRender* dialog = new DialogRender(this, &_renderer);
    dialog->startRender(3, 400, 300);

    delete dialog;
}

void FormRender::startRender()
{
    if (_renderer_inited)
    {
        rendererDelete(&_renderer);
    }
    _renderer = sceneryCreateStandardRenderer();
    _renderer_inited = true;
    
    DialogRender* dialog = new DialogRender(this, &_renderer);
    dialog->startRender(_quality, _width, _height);

    delete dialog;
}

void FormRender::showRender()
{
    if (_renderer_inited)
    {
        DialogRender* dialog = new DialogRender(this, &_renderer);
        dialog->loadLastRender();

        delete dialog;
    }
}

void FormRender::saveRender()
{
    if (_renderer_inited)
    {
        QString filepath;

        filepath = QFileDialog::getSaveFileName(this, tr("Choose a filename to save the last render"));
        if (!filepath.isNull())
        {
            renderSaveToFile(_renderer.render_area, (char*)filepath.toStdString().c_str());
            QMessageBox::information(this, "Message", QString(tr("The picture %1 has been saved.")).arg(filepath));
        }
    }
}
