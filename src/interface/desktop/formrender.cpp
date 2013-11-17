#include "formrender.h"

#include <QPushButton>

#include "dialogrender.h"
#include "inputcamera.h"
#include "tools.h"
#include "render.h"
#include "atmosphere/public.h"
#include "clouds/public.h"
#include "terrain/public.h"
#include "water/public.h"
#include "RenderingScenery.h"
#include "PackStream.h"
#include "SoftwareRenderer.h"
#include "BasePreview.h"
#include "CloudsDefinition.h"
#include "CameraDefinition.h"

/**************** Previews ****************/
class PreviewRenderLandscape : public BasePreview
{
public:
    PreviewRenderLandscape(QWidget* parent) : BasePreview(parent)
    {
        _renderer = new SoftwareRenderer();
        _renderer->getCameraLocation = _getCameraLocation;
        lightingManagerDisableSpecularity(_renderer->lighting);

        _no_clouds = new CloudsDefinition(NULL);
        _clouds_enabled = true;

        addOsd(QString("geolocation"));
        addToggle("clouds", tr("Clouds"), false);

        configHdrToneMapping(true);
        configScaling(0.5, 200.0, 3.0, 50.0);
        configScrolling(-1000.0, 1000.0, 0.0, -1000.0, 1000.0, 0.0);
    }
    ~PreviewRenderLandscape()
    {
        delete _renderer;
        delete _no_clouds;
    }

protected:
    Color getColor(double x, double y)
    {
        Vector3 location;
        double height = _renderer->terrain->getHeight(_renderer, x, y, 1);

        if (height < _renderer->water->getHeightInfo(_renderer).max_height)
        {
            return _renderer->water->getResult(_renderer, x, y).final;
        }
        else
        {
            location.x = x;
            location.y = height;
            location.z = y;
            return _renderer->terrain->getFinalColor(_renderer, location, scaling);
        }
    }

    void updateData()
    {
        RenderingScenery::getCurrent()->bindToRenderer(_renderer);
        if (!_clouds_enabled)
        {
            CloudsRendererClass.bind(_renderer, _no_clouds);
        }

        _renderer->atmosphere->applyAerialPerspective = _applyAerialPerspective;
    }

    void toggleChangeEvent(QString key, bool value)
    {
        if (key == "clouds")
        {
            _clouds_enabled = value;
            redraw();
        }
    }

private:
    SoftwareRenderer* _renderer;
    bool _clouds_enabled;
    CloudsDefinition* _no_clouds;

    static Vector3 _getCameraLocation(Renderer*, Vector3 location)
    {
        return v3Add(location, v3Scale(VECTOR_UP, 50.0));
    }

    static AtmosphereResult _applyAerialPerspective(Renderer*, Vector3, Color base)
    {
        AtmosphereResult result;
        atmosphereInitResult(&result);
        result.base = base;
        result.final = base;
        atmosphereUpdateResult(&result);
        return result;
    }
};

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

    _preview_landscape = new PreviewRenderLandscape(this);
    addPreview(_preview_landscape, QString(tr("Top-down preview")));

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
        rendererDelete(_renderer);
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
    RenderingScenery::getCurrent()->getCamera(_camera);
    BaseForm::revertConfig();
}

void FormRender::applyConfig()
{
    RenderingScenery::getCurrent()->setCamera(_camera);
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
        rendererDelete(_renderer);
    }
    _renderer = new SoftwareRenderer(RenderingScenery::getCurrent());
    _renderer_inited = true;

    DialogRender* dialog = new DialogRender(this, _renderer);
    RenderParams params = {400, 300, 1, 3};
    dialog->startRender(params);

    delete dialog;
}

void FormRender::startRender()
{
    if (_renderer_inited)
    {
        rendererDelete(_renderer);
    }
    _renderer = new SoftwareRenderer(RenderingScenery::getCurrent());
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
