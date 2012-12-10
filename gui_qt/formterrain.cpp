#include "formterrain.h"

#include <math.h>
#include <QColor>
#include <QSlider>
#include "tools.h"

#include "../lib_paysages/scenery.h"
#include "../lib_paysages/euclid.h"
#include "dialogheightmap.h"

static TerrainDefinition* _definition;

/**************** Previews ****************/
class PreviewTerrainHeight:public BasePreview
{
public:
    PreviewTerrainHeight(QWidget* parent):BasePreview(parent)
    {
        _renderer = rendererCreate();

        addOsd(QString("geolocation"));

        configScaling(0.5, 200.0, 3.0, 50.0);
        configScrolling(-1000.0, 1000.0, 0.0, -1000.0, 1000.0, 0.0);
    }
protected:
    QColor getColor(double x, double y)
    {
        double height = 0.0;

        // TODO
        //height = terrainGetHeightNormalized(&_preview_definition, x, y);
        return QColor((int)(255.0 * height), (int)(255.0 * height), (int)(255.0 * height));
    }
    void updateData()
    {
        TerrainRendererClass.bind(_renderer.terrain, _definition);
    }
private:
    Renderer _renderer;
};

class PreviewTerrainColor:public BasePreview
{
public:
    PreviewTerrainColor(QWidget* parent):BasePreview(parent)
    {
        LightDefinition light;
        TextureLayerDefinition* texture;

        _renderer = rendererCreate();
        _renderer.render_quality = 3;
        _renderer.applyTextures = _applyTextures;
        _renderer.getLightStatus = _getLightStatus;
        _renderer.alterLight = _alterLight;
        _renderer.camera_location.x = 0.0;
        _renderer.camera_location.y = 50.0;
        _renderer.camera_location.z = 0.0;

        _lighting = lightingCreateDefinition();
        light.color.r = 0.6;
        light.color.g = 0.6;
        light.color.b = 0.6;
        light.direction.x = -1.0;
        light.direction.y = -0.5;
        light.direction.z = 1.0;
        light.direction = v3Normalize(light.direction);
        light.filtered = 0;
        light.masked = 1;
        light.reflection = 1.0;
        lightingAddLight(&_lighting, light);
        light.color.r = 0.3;
        light.color.g = 0.3;
        light.color.b = 0.3;
        light.direction.x = 0.5;
        light.direction.y = 0.7071;
        light.direction.z = -0.5;
        light.filtered = 0;
        light.masked = 0;
        light.reflection = 0.0;
        lightingAddLight(&_lighting, light);
        lightingValidateDefinition(&_lighting);

        _textures = texturesCreateDefinition();
        texture = (TextureLayerDefinition*)layersGetLayer(_textures.layers, layersAddLayer(_textures.layers, NULL));
        texture->material.base = COLOR_WHITE;
        texture->material.reflection = 0.3;
        texture->material.shininess = 2.0;
        texture->bump_height = 0.0;
        texturesLayerValidateDefinition(texture);

        _renderer.customData[1] = &_textures;
        _renderer.customData[2] = &_lighting;

        addOsd(QString("geolocation"));

        configScaling(0.5, 200.0, 3.0, 50.0);
        configScrolling(-1000.0, 1000.0, 0.0, -1000.0, 1000.0, 0.0);
    }
protected:
    QColor getColor(double x, double y)
    {
        Vector3 point;

        point.x = x;
        point.y = _renderer.terrain->getHeight(&_renderer, x, y);
        point.z = y;

        return colorToQColor(_renderer.terrain->getFinalColor(&_renderer, point, scaling));
    }
    void updateData()
    {
        TerrainRendererClass.bind(_renderer.terrain, _definition);
        //sceneryGetTextures(&_textures);
    }
private:
    Renderer _renderer;
    TexturesDefinition _textures;
    LightingDefinition _lighting;

    static Color _applyTextures(Renderer* renderer, Vector3 location, double precision)
    {
        return texturesGetColor((TexturesDefinition*)(renderer->customData[1]), renderer, location.x, location.z, precision);
    }

    static void _getLightStatus(Renderer* renderer, LightStatus* status, Vector3 location)
    {
        lightingGetStatus((LightingDefinition*)renderer->customData[2], renderer, location, status);
    }

    static void _alterLight(Renderer* renderer, LightDefinition* light, Vector3 location)
    {
        *light = renderer->terrain->alterLight(renderer, light, location);
    }

};

/**************** Form ****************/
FormTerrain::FormTerrain(QWidget *parent):
    BaseForm(parent)
{
    _definition = (TerrainDefinition*)TerrainDefinitionClass.create();

    QPushButton* button = addButton(tr("Paint"));
    connect(button, SIGNAL(clicked()), this, SLOT(startPainting()));

    previewHeight = new PreviewTerrainHeight(this);
    previewColor = new PreviewTerrainColor(this);
    addPreview(previewHeight, tr("Height preview (normalized)"));
    addPreview(previewColor, tr("Lighted preview (no texture)"));

    //addInputNoise(tr("Noise"), _definition.height_noise);
    addInputDouble(tr("Height"), &_definition->height, 0.0, 20.0, 0.1, 1.0);
    addInputDouble(tr("Scaling"), &_definition->scaling, 20.0, 200.0, 1.0, 10.0);
    addInputDouble(tr("Shadow smoothing"), &_definition->shadow_smoothing, 0.0, 0.3, 0.003, 0.03);

    revertConfig();
}

void FormTerrain::revertConfig()
{
    sceneryGetTerrain(_definition);
    BaseForm::revertConfig();
}

void FormTerrain::applyConfig()
{
    scenerySetTerrain(_definition);
    BaseForm::applyConfig();
}

void FormTerrain::configChangeEvent()
{
    TerrainDefinitionClass.validate(_definition);
    BaseForm::configChangeEvent();
}

void FormTerrain::startPainting()
{
    DialogHeightMap::editHeightMap(this, _definition);
}
