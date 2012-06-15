#include "formrender.h"

#include <QFileDialog>
#include <QMessageBox>
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
        _renderer = rendererCreate();
        _renderer.applyTextures = _applyTextures;
        _renderer.getTerrainHeight = _getTerrainHeight;
        _renderer.alterLight = _alterLight;
        _renderer.getLightStatus = _getLightStatus;
        _renderer.camera_location.x = 0.0;
        _renderer.camera_location.y = 50.0;
        _renderer.camera_location.z = 0.0;

        _terrain = terrainCreateDefinition();
        _textures = texturesCreateDefinition();
        _lighting = lightingCreateDefinition();
        _water = waterCreateDefinition();

        _renderer.customData[0] = &_terrain;
        _renderer.customData[1] = &_textures;
        _renderer.customData[2] = &_lighting;
        _renderer.customData[3] = &_water;
        
        configScaling(0.5, 200.0, 3.0, 50.0);
        configScrolling(-1000.0, 1000.0, 0.0, -1000.0, 1000.0, 0.0);
    }
protected:
    QColor getColor(float x, float y)
    {
        Vector3 down = {0.0, -1.0, 0.0};
        Vector3 location;
        float height = terrainGetHeight(&_terrain, x, y);
        
        if (height < _water.height)
        {
            location.x = x;
            location.y = _water.height;
            location.z = y;
            return colorToQColor(waterGetColor(&_water, &_renderer, location, down));
        }
        else
        {
            return colorToQColor(terrainGetColor(&_terrain, &_renderer, x, y, scaling));
        }
    }
    void updateData()
    {
        sceneryGetTerrain(&_terrain);
        sceneryGetLighting(&_lighting);
        sceneryGetTextures(&_textures);
        sceneryGetWater(&_water);
    }
private:
    Renderer _renderer;
    TerrainDefinition _terrain;
    WaterDefinition _water;
    TexturesDefinition _textures;
    LightingDefinition _lighting;

    static float _getTerrainHeight(Renderer* renderer, float x, float z)
    {
        return terrainGetHeight((TerrainDefinition*)(renderer->customData[0]), x, z);
    }

    static Color _applyTextures(Renderer* renderer, Vector3 location, float precision)
    {
        return texturesGetColor((TexturesDefinition*)(renderer->customData[1]), renderer, location.x, location.z, precision);
    }
    
    static void _alterLight(Renderer* renderer, LightDefinition* light, Vector3 location)
    {
        light->color = terrainLightFilter((TerrainDefinition*)(renderer->customData[0]), renderer, light->color, location, v3Scale(light->direction, -1000.0), v3Scale(light->direction, -1.0));
    }

    static void _getLightStatus(Renderer* renderer, LightStatus* status, Vector3 location)
    {
        lightingGetStatus((LightingDefinition*)renderer->customData[2], renderer, location, status);
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
    RenderParams params = {400, 300, 1, 3};
    dialog->startRender(params);

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
    dialog->startRender(_params);

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

        filepath = QFileDialog::getSaveFileName(this, tr("Paysages 3D - Choose a filename to save the last render"), QString(), tr("Images (*.png *.jpg)"));
        if (!filepath.isNull())
        {
            if (!filepath.toLower().endsWith(".jpg") && !filepath.toLower().endsWith(".jpeg") && !filepath.toLower().endsWith(".png"))
            {
                filepath = filepath.append(".png");
            }
            if (renderSaveToFile(_renderer.render_area, (char*)filepath.toStdString().c_str()))
            {
                QMessageBox::information(this, "Message", QString(tr("The picture %1 has been saved.")).arg(filepath));
            }
            else
            {
                QMessageBox::critical(this, "Message", QString(tr("Can't write to file : %1")).arg(filepath));
            }
        }
    }
}
