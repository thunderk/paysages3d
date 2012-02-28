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
        _renderer = rendererCreate();
        _renderer.applyTextures = _applyTextures;
        _renderer.getTerrainHeight = _getTerrainHeight;
        /*_renderer.applyLightingToSurface = _applyLightingToSurface;*/

        _terrain = terrainCreateDefinition();
        _textures = texturesCreateDefinition();

        _renderer.customData[0] = &_terrain;
        _renderer.customData[1] = &_textures;
    }
protected:
    QColor getColor(double x, double y)
    {
        return colorToQColor(terrainGetColor(&_terrain, &_renderer, x, y, scaling));
    }
    void updateData()
    {
        terrainCopyDefinition(&_definition, &_terrain);
        sceneryGetTextures(&_textures);
    }
private:
    Renderer _renderer;
    TerrainDefinition _terrain;
    TexturesDefinition _textures;

    static double _getTerrainHeight(Renderer* renderer, double x, double z)
    {
        return terrainGetHeight((TerrainDefinition*)(renderer->customData[0]), x, z);
    }

    static Color _applyTextures(Renderer* renderer, Vector3 location, double precision)
    {
        return texturesGetColor((TexturesDefinition*)(renderer->customData[1]), renderer, location, precision);
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
    addPreview(previewColor, QString(tr("Textured preview (no shadow)")));

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
