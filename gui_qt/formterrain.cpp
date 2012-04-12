#include "formterrain.h"

#include "tools.h"
#include <QColor>
#include <QSlider>
#include <math.h>

#include "../lib_paysages/terrain.h"
#include "../lib_paysages/scenery.h"
#include "../lib_paysages/shared/constants.h"

static TerrainDefinition _definition;

/**************** Previews ****************/
class PreviewTerrainHeight:public BasePreview
{
public:
    PreviewTerrainHeight(QWidget* parent):BasePreview(parent)
    {
        _preview_definition = terrainCreateDefinition();
        
        configScaling(0.5, 200.0, 1.0, 50.0);
    }
protected:
    QColor getColor(double x, double y)
    {
        double height;

        height = terrainGetHeightNormalized(&_preview_definition, x, y);
        return QColor((int)(255.0 * height), (int)(255.0 * height), (int)(255.0 * height));
    }
    void updateData()
    {
        terrainCopyDefinition(&_definition, &_preview_definition);
    }
private:
    TerrainDefinition _preview_definition;
};

class PreviewTerrainColor:public BasePreview
{
public:
    PreviewTerrainColor(QWidget* parent):BasePreview(parent)
    {
        LightDefinition light;
        TextureLayerDefinition* texture;

        _renderer = rendererCreate();
        _renderer.applyTextures = _applyTextures;
        _renderer.getTerrainHeight = _getTerrainHeight;
        _renderer.applyLightingToSurface = _applyLightingToSurface;
        _renderer.maskLight = _maskLight;
        _renderer.camera_location.x = 0.0;
        _renderer.camera_location.y = 50.0;
        _renderer.camera_location.z = 0.0;

        _lighting = lightingCreateDefinition();
        light.color.r = 0.6;
        light.color.g = 0.6;
        light.color.b = 0.6;
        light.amplitude = 0.0;
        light.direction.x = -0.5;
        light.direction.y = -0.7071;
        light.direction.z = 0.5;
        light.filtered = 0;
        light.masked = 1;
        light.reflection = 1.0;
        lightingAddLight(&_lighting, light);
        light.color.r = 0.3;
        light.color.g = 0.3;
        light.color.b = 0.3;
        light.amplitude = 0.0;
        light.direction.x = 0.5;
        light.direction.y = 0.7071;
        light.direction.z = -0.5;
        light.filtered = 0;
        light.masked = 0;
        light.reflection = 0.0;
        lightingAddLight(&_lighting, light);
        lightingValidateDefinition(&_lighting);

        _terrain = terrainCreateDefinition();
        _textures = texturesCreateDefinition();
        texturesAddLayer(&_textures);
        texture = texturesGetLayer(&_textures, 0);
        texture->material.base = COLOR_WHITE;
        texture->material.reflection = 0.3;
        texture->material.shininess = 2.0;
        texture->bump_height = 0.0;
        texturesLayerValidateDefinition(texture);

        _renderer.customData[0] = &_terrain;
        _renderer.customData[1] = &_textures;
        _renderer.customData[2] = &_lighting;
        
        configScaling(0.5, 200.0, 1.0, 50.0);
    }
protected:
    QColor getColor(double x, double y)
    {
        return colorToQColor(terrainGetColor(&_terrain, &_renderer, x, y, scaling));
    }
    void updateData()
    {
        terrainCopyDefinition(&_definition, &_terrain);
        //sceneryGetTextures(&_textures);
    }
private:
    Renderer _renderer;
    TerrainDefinition _terrain;
    TexturesDefinition _textures;
    LightingDefinition _lighting;

    static double _getTerrainHeight(Renderer* renderer, double x, double z)
    {
        return terrainGetHeight((TerrainDefinition*)(renderer->customData[0]), x, z);
    }

    static Color _applyTextures(Renderer* renderer, Vector3 location, double precision)
    {
        return texturesGetColor((TexturesDefinition*)(renderer->customData[1]), renderer, location, precision);
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
FormTerrain::FormTerrain(QWidget *parent):
    BaseForm(parent)
{
    _definition = terrainCreateDefinition();

    previewHeight = new PreviewTerrainHeight(this);
    previewColor = new PreviewTerrainColor(this);
    addPreview(previewHeight, QString(tr("Height preview (normalized)")));
    addPreview(previewColor, QString(tr("Lighted preview (no texture)")));

    addInputNoise(tr("Noise"), _definition.height_noise);
    addInputDouble(tr("Height"), &_definition.height_factor, 0.0, 20.0, 0.1, 1.0);
    addInputDouble(tr("Scaling"), &_definition.scaling, 1.0, 20.0, 0.1, 1.0);

    revertConfig();
}

void FormTerrain::revertConfig()
{
    sceneryGetTerrain(&_definition);
    BaseForm::revertConfig();
}

void FormTerrain::applyConfig()
{
    scenerySetTerrain(&_definition);
    BaseForm::applyConfig();
}

void FormTerrain::configChangeEvent()
{
    terrainValidateDefinition(&_definition);
    BaseForm::configChangeEvent();
}
