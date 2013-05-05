#include "formterrain.h"

#include <QColor>
#include <QSlider>
#include "tools.h"
#include "terrain/dialogterrainpainting.h"
#include "rendering/scenery.h"

static TerrainDefinition* _definition;

/**************** Previews ****************/
class PreviewTerrainColor:public BasePreview
{
public:
    PreviewTerrainColor(QWidget* parent):BasePreview(parent)
    {
        _renderer = terrainCreatePreviewRenderer();

        addOsd(QString("geolocation"));

        configScaling(20.0, 1000.0, 20.0, 50.0);
        configScrolling(-1000.0, 1000.0, 0.0, -1000.0, 1000.0, 0.0);
    }
protected:
    Color getColor(double x, double y)
    {
        return terrainGetPreviewColor(_renderer, x, y, scaling);
    }
    void updateData()
    {
        TerrainRendererClass.bind(_renderer, _definition);
    }
private:
    Renderer* _renderer;
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
    addInputDouble(tr("Scaling"), &_definition->scaling, 0.1, 3.0, 0.03, 0.3);
    addInputDouble(tr("Height modifier"), &_definition->height, 0.0, 3.0, 0.01, 0.1);
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
    DialogTerrainPainting* dialog = new DialogTerrainPainting(this, _definition);
    if (dialog->exec())
    {
        configChangeEvent();
    }
    delete dialog;
}
