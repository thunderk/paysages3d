#include "dialogterrainpainting.h"
#include "ui_dialogterrainpainting.h"

#include "tools.h"

DialogTerrainPainting::DialogTerrainPainting(QWidget*parent, TerrainDefinition* terrain) :
    QDialog(parent),
    ui(new Ui::DialogTerrainPainting)
{
    ui->setupUi(this);

    _terrain_original = terrain;
    _terrain_modified = (TerrainDefinition*)TerrainDefinitionClass.create();

    revert();

    brushConfigChanged();
}

DialogTerrainPainting::~DialogTerrainPainting()
{
    delete ui;
}

void DialogTerrainPainting::accept()
{
    TerrainDefinitionClass.copy(_terrain_modified, _terrain_original);
    QDialog::accept();
}

void DialogTerrainPainting::revert()
{
    TerrainDefinitionClass.copy(_terrain_original, _terrain_modified);

    WidgetHeightMap* heightmap = findChild<WidgetHeightMap*>("widget_heightmap");
    if (heightmap)
    {
        heightmap->setTerrain(_terrain_modified);
        heightmap->setBrush(&_brush);
    }
}

void DialogTerrainPainting::brushConfigChanged()
{
    QLabel* label;
    QComboBox* combobox;
    QSlider* slider;

    // Fill brush object
    combobox = findChild<QComboBox*>("input_brush_mode");
    if (combobox)
    {
        _brush.setMode((PaintingBrushMode)combobox->currentIndex());
    }
    slider = findChild<QSlider*>("input_brush_size");
    if (slider)
    {
        _brush.setSize(slider);
    }
    slider = findChild<QSlider*>("input_brush_smoothing");
    if (slider)
    {
        _brush.setSmoothing(slider);
    }
    slider = findChild<QSlider*>("input_brush_strength");
    if (slider)
    {
        _brush.setStrength(slider);
    }

    // Update brush description
    label = findChild<QLabel*>("label_brush_description");
    if (label)
    {
        label->setText(getHelpText());
    }

    // Update brush preview

    // Tell to 3D widget
    WidgetHeightMap* heightmap = findChild<WidgetHeightMap*>("widget_heightmap");
    if (heightmap)
    {
        heightmap->setBrush(&_brush);
    }
}

void DialogTerrainPainting::heightmapChanged()
{
    QLabel* label = findChild<QLabel*>("label_memory_consumption");
    if (label)
    {
        qint64 memused = terrainGetMemoryStats(_terrain_modified);
        label->setText(tr("Memory used: %1").arg(getHumanMemory(memused)));

        // TODO Find available memory
        QProgressBar* progress = findChild<QProgressBar*>("progress_memory_consumption");
        if (progress)
        {
            progress->setMaximum(1024);
            progress->setValue(memused / 1024);
        }
    }
}

QString DialogTerrainPainting::getHelpText()
{
    QString result = _brush.getHelpText();
    return result;
}
