#include "formterrain.h"

#include "tools.h"
#include <QColor>
#include <QSlider>
#include <math.h>

#include "../lib_paysages/terrain.h"
#include "../lib_paysages/scenery.h"
#include "../lib_paysages/shared/functions.h"
#include "../lib_paysages/shared/constants.h"

static TerrainDefinition _definition;

/**************** Previews ****************/
class PreviewTerrainHeight:public Preview
{
public:
    PreviewTerrainHeight(QWidget* parent):Preview(parent)
    {
    }
protected:
    QColor getColor(double x, double y)
    {
        double height;

        height = terrainGetHeightNormalized(&_definition, x, y);
        return QColor((int)(255.0 * height), (int)(255.0 * height), (int)(255.0 * height));
    }
};

class PreviewTerrainColor:public Preview
{
public:
    PreviewTerrainColor(QWidget* parent):Preview(parent)
    {
        // TODO Use custom renderer
        _renderer = sceneryGetStandardRenderer(3);
    }
protected:
    QColor getColor(double x, double y)
    {
        return colorToQColor(terrainGetColor(&_definition, &_renderer, x, y, scaling));
    }
private:
    Renderer _renderer;
};

/**************** Form ****************/
FormTerrain::FormTerrain(QWidget *parent):
    BaseForm(parent)
{
    _definition = terrainCreateDefinition();

    previewHeight = new PreviewTerrainHeight(this);
    previewColor = new PreviewTerrainColor(this);
    addPreview(previewHeight, QString("Height preview (normalized)"));
    addPreview(previewColor, QString("Textured preview (no shadow)"));

    addInputNoise("Noise", _definition.height_noise);
    addInputDouble("Height", &_definition.height_factor, 0.0, 20.0, 0.1, 1.0);
    addInputDouble("Scaling", &_definition.scaling, 1.0, 20.0, 0.1, 1.0);

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

void FormTerrain::applyConfigPreview()
{
    terrainValidateDefinition(&_definition);
    BaseForm::applyConfigPreview();
}