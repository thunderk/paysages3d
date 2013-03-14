#include "formrender.h"

#include "dialogrender.h"
#include "inputcamera.h"
#include "tools.h"
#include "../lib_paysages/render.h"
#include "../lib_paysages/scenery.h"

/**************** Previews ****************/
class PreviewRenderLandscape:public BasePreview
{
public:
    PreviewRenderLandscape(QWidget* parent):BasePreview(parent)
    {
        _renderer = sceneryCreateStandardRenderer();
        _renderer->applyTextures = _applyTextures;
        _renderer->getCameraLocation = _getCameraLocation;
        lightingManagerDisableSpecularity(_renderer->lighting);

        _textures = texturesCreateDefinition();

        _atmosphere = (AtmosphereDefinition*)AtmosphereDefinitionClass.create();
        _terrain = (TerrainDefinition*)TerrainDefinitionClass.create();
        _water = (WaterDefinition*)WaterDefinitionClass.create();
        _clouds = (CloudsDefinition*)CloudsDefinitionClass.create();

        _renderer->customData[0] = &_textures;

        _clouds_enabled = true;

        addOsd(QString("geolocation"));
        addToggle("clouds", tr("Clouds"), false);

        configHdrToneMapping(true);
        configScaling(0.5, 200.0, 3.0, 50.0);
        configScrolling(-1000.0, 1000.0, 0.0, -1000.0, 1000.0, 0.0);
    }
protected:
    Color getColor(double x, double y)
    {
        Vector3 location;
        double height = _renderer->terrain->getHeight(_renderer, x, y, 1);

        if (height < _water->height)
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
        sceneryGetTextures(&_textures);

        sceneryBindRenderer(_renderer);
        _renderer->atmosphere->applyAerialPerspective = _applyAerialPerspective;

        if (!_clouds_enabled)
        {
            CloudsRendererClass.bind(_renderer, _clouds);
        }
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
    Renderer* _renderer;
    bool _clouds_enabled;
    CloudsDefinition* _clouds;
    TerrainDefinition* _terrain;
    WaterDefinition *_water;
    TexturesDefinition _textures;
    AtmosphereDefinition* _atmosphere;

    static Color _applyTextures(Renderer* renderer, Vector3 location, double precision)
    {
        return texturesGetColor((TexturesDefinition*)(renderer->customData[0]), renderer, location.x, location.z, precision);
    }

    static Vector3 _getCameraLocation(Renderer*, Vector3 location)
    {
        return v3Add(location, v3Scale(VECTOR_UP, 50.0));
    }

    static AtmosphereResult _applyAerialPerspective(Renderer*, Vector3, Color base)
    {
        AtmosphereResult result;
        result.base = base;
        result.distance = 0.0;
        result.inscattering = COLOR_BLACK;
        result.attenuation = COLOR_BLACK;
        result.final = base;
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
    _camera = cameraCreateDefinition();

    _renderer_inited = false;

    disablePreviewsUpdate();

    _preview_landscape = new PreviewRenderLandscape(this);
    addPreview(_preview_landscape, QString(tr("Top-down preview")));

    addInput(new InputCamera(this, tr("Camera"), &_camera));
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
    if (_renderer_inited)
    {
        rendererDelete(_renderer);
    }
}

void FormRender::savePack(PackStream* stream)
{
    BaseForm::savePack(stream);

    packWriteInt(stream, &_params.width);
    packWriteInt(stream, &_params.height);
    packWriteInt(stream, &_params.antialias);
    packWriteInt(stream, &_params.quality);
}

void FormRender::loadPack(PackStream* stream)
{
    BaseForm::loadPack(stream);

    packReadInt(stream, &_params.width);
    packReadInt(stream, &_params.height);
    packReadInt(stream, &_params.antialias);
    packReadInt(stream, &_params.quality);

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

void FormRender::startQuickRender()
{
    if (_renderer_inited)
    {
        rendererDelete(_renderer);
    }
    _renderer = sceneryCreateStandardRenderer();
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
    _renderer = sceneryCreateStandardRenderer();
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
