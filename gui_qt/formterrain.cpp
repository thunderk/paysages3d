#include "formterrain.h"

#include <QColor>
#include <QSlider>
#include "tools.h"
#include "dialogheightmap.h"
#include "../lib_paysages/scenery.h"

static TerrainDefinition* _definition;

/**************** Previews ****************/
class PreviewTerrainColor:public BasePreview
{
public:
    PreviewTerrainColor(QWidget* parent):BasePreview(parent)
    {
        _renderer = terrainCreatePreviewRenderer();

        addOsd(QString("geolocation"));

        configScaling(0.5, 200.0, 3.0, 50.0);
        configScrolling(-1000.0, 1000.0, 0.0, -1000.0, 1000.0, 0.0);
    }
protected:
    QColor getColor(double x, double y)
    {
        return colorToQColor(terrainGetPreviewColor(&_renderer, x, -y, scaling));
    }
    void updateData()
    {
        TerrainRendererClass.bind(_renderer.terrain, _definition);
    }
private:
    Renderer _renderer;
};

/**************** Form ****************/
FormTerrain::FormTerrain(QWidget *parent):
    BaseForm(parent)
{
    _definition = (TerrainDefinition*)TerrainDefinitionClass.create();

    QPushButton* button = addButton(tr("Paint"));
    connect(button, SIGNAL(clicked()), this, SLOT(startPainting()));

    /*previewHeight = new PreviewTerrainHeight(this);*/
    previewColor = new PreviewTerrainColor(this);
    /*addPreview(previewHeight, tr("Height preview (normalized)"));*/
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
