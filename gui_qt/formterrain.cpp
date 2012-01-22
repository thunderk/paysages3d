#include "formterrain.h"

#include "tools.h"
#include <QColor>
#include <QSlider>
#include <math.h>

#include "../lib_paysages/terrain.h"
#include "../lib_paysages/shared/functions.h"
#include "../lib_paysages/shared/constants.h"

static TerrainDefinition _definition;

/**************** Previews ****************/
class PreviewTerrainHeight:public Preview
{
public:
    PreviewTerrainHeight(QWidget* parent):
        Preview(parent)
    {
    }
protected:
    QColor getColor(double x, double y)
    {
        double height;

        height = terrainGetHeightNormalized(x, y);
        return QColor((int)(255.0 * height), (int)(255.0 * height), (int)(255.0 * height));
    }
};

class PreviewTerrainColor:public Preview
{
public:
    PreviewTerrainColor(QWidget* parent):
        Preview(parent)
    {
        _environment.toggle_fog = 0;
    }
protected:
    QColor getColor(double x, double y)
    {
        return colorToQColor(terrainGetColorCustom(x, y, scaling, &_definition, NULL, &_environment));
    }
private:
    TerrainEnvironment _environment;
};

/**************** Form ****************/
FormTerrain::FormTerrain(QWidget *parent):
    BaseForm(parent)
{
    _definition = terrainCreateDefinition();

    previewHeight = new PreviewTerrainHeight(this);
    previewColor = new PreviewTerrainColor(this);
    addPreview(previewHeight, QString("Height preview"));
    addPreview(previewColor, QString("Color preview"));

    addInputNoise("Height", _definition.height_noise);

    revertConfig();
}

void FormTerrain::revertConfig()
{
    terrainCopyDefinition(terrainGetDefinition(), &_definition);
    BaseForm::revertConfig();
}

void FormTerrain::applyConfig()
{
    terrainSetDefinition(_definition);
    BaseForm::applyConfig();
}
