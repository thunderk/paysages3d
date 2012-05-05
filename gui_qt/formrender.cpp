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
        _renderer.applyLightingToSurface = _applyLightingToSurface;
        _renderer.maskLight = _maskLight;
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
        
        configScaling(0.5, 200.0, 1.0, 50.0);
        configScrolling(-1000.0, 1000.0, 0.0, -1000.0, 1000.0, 0.0);
    }
protected:
    QColor getColor(double x, double y)
    {
        Vector3 down = {0.0, -1.0, 0.0};
        Vector3 location;
        double height = terrainGetHeight(&_terrain, x, y);
        
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

    static double _getTerrainHeight(Renderer* renderer, double x, double z)
    {
        return terrainGetHeight((TerrainDefinition*)(renderer->customData[0]), x, z);
    }

    static Color _applyTextures(Renderer* renderer, Vector3 location, double precision)
    {
        return texturesGetColor((TexturesDefinition*)(renderer->customData[1]), renderer, location.x, location.z, precision);
    }
    
    static Color _applyLightingToSurface(Renderer* renderer, Vector3 location, Vector3 normal, SurfaceMaterial material)
    {
        return lightingApplyToSurface((LightingDefinition*)renderer->customData[2], renderer, location, normal, material);
    }
    
    static Color _maskLight(Renderer* renderer, Color light_color, Vector3 at_location, Vector3 light_location, Vector3 direction_to_light)
    {
        return terrainLightFilter((TerrainDefinition*)(renderer->customData[0]), renderer, light_color, at_location, light_location, direction_to_light);
    }
};

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

    _preview_landscape = new PreviewRenderLandscape(this);
    addPreview(_preview_landscape, QString(tr("Top-down preview")));
    
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

        filepath = QFileDialog::getSaveFileName(this, tr("Paysages 3D - Choose a filename to save the last render"), QString(), tr("Images (*.png *.jpg)"));
        if (!filepath.isNull())
        {
            if (!filepath.toLower().endsWith(".jpg") && !filepath.toLower().endsWith(".jpeg") && !filepath.toLower().endsWith(".png"))
            {
                filepath = filepath.append(".png");
            }
            renderSaveToFile(_renderer.render_area, (char*)filepath.toStdString().c_str());
            QMessageBox::information(this, "Message", QString(tr("The picture %1 has been saved.")).arg(filepath));
        }
    }
}
