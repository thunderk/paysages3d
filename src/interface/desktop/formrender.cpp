#include "formrender.h"

#include <QPushButton>

#include "dialogrender.h"
#include "inputcamera.h"
#include "tools.h"
#include "DesktopScenery.h"
#include "PackStream.h"
#include "SoftwareRenderer.h"
#include "BasePreview.h"
#include "CloudsDefinition.h"
#include "CameraDefinition.h"
#include "SceneryTopDownPreviewRenderer.h"

/**************** Form ****************/
FormRender::FormRender(QWidget *parent) :
BaseForm(parent, true)
{
    QPushButton* button;

    _params.quality = 5;
    _params.width = 800;
    _params.height = 600;
    _params.antialias = 1;
    _camera = new CameraDefinition;

    _renderer_inited = false;

    disablePreviewsUpdate();

    _preview_landscape = new BasePreview(this);
    _preview_landscape_renderer = new SceneryTopDownPreviewRenderer(DesktopScenery::getCurrent());
    addPreview(_preview_landscape, QString(tr("Top-down preview")));
    _preview_landscape->setRenderer(_preview_landscape_renderer);

    addInput(new InputCamera(this, tr("Camera"), _camera));
    addInputInt(tr("Quality"), &_params.quality, 1, 10, 1, 1);
    addInputInt(tr("Image width"), &_params.width, 100, 2000, 10, 100);
    addInputInt(tr("Image height"), &_params.height, 100, 1200, 10, 100);
    addInputInt(tr("Anti aliasing"), &_params.antialias, 1, 4, 1, 1);

    button = addButton(tr("Start new render"));
    connect(button, SIGNAL(clicked()), this, SLOT(startRender()));
    button = addButton(tr("Show last render"));
    connect(button, SIGNAL(clicked()), this, SLOT(showRender()));

    revertConfig();
}

FormRender::~FormRender()
{
    delete _camera;
    if (_renderer_inited)
    {
        delete _renderer;
    }
}

void FormRender::savePack(PackStream* stream)
{
    BaseForm::savePack(stream);

    stream->write(&_params.width);
    stream->write(&_params.height);
    stream->write(&_params.antialias);
    stream->write(&_params.quality);
}

void FormRender::loadPack(PackStream* stream)
{
    BaseForm::loadPack(stream);

    stream->read(&_params.width);
    stream->read(&_params.height);
    stream->read(&_params.antialias);
    stream->read(&_params.quality);

    revertConfig();
}

void FormRender::revertConfig()
{
    DesktopScenery::getCurrent()->getCamera(_camera);
    BaseForm::revertConfig();
}

void FormRender::applyConfig()
{
    DesktopScenery::getCurrent()->setCamera(_camera);
    BaseForm::applyConfig();
}

void FormRender::configChangeEvent()
{
    _camera->validate();
    BaseForm::configChangeEvent();
}

void FormRender::startQuickRender()
{
    if (_renderer_inited)
    {
        delete _renderer;
    }
    _renderer = new SoftwareRenderer(DesktopScenery::getCurrent());
    _renderer_inited = true;

    DialogRender* dialog = new DialogRender(this, _renderer);
    RenderArea::RenderParams params = {400, 300, 1, 3};
    dialog->startRender(params);

    delete dialog;
}

void FormRender::startRender()
{
    if (_renderer_inited)
    {
        delete _renderer;
    }
    _renderer = new SoftwareRenderer(DesktopScenery::getCurrent());
    _renderer_inited = true;

    DialogRender* dialog = new DialogRender(this, _renderer);
    dialog->startRender(_params);

    delete dialog;
}

void FormRender::showRender()
{
    if (_renderer_inited)
    {
        DialogRender* dialog = new DialogRender(this, _renderer);
        dialog->loadLastRender();

        delete dialog;
    }
}
