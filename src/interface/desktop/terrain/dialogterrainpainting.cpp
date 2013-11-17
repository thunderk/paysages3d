#include "dialogterrainpainting.h"
#include "ui_dialogterrainpainting.h"

#include <QKeyEvent>
#include <QWheelEvent>
#include <QMessageBox>
#include "tools.h"
#include "TerrainDefinition.h"

DialogTerrainPainting::DialogTerrainPainting(QWidget*parent, TerrainDefinition* terrain) :
    QDialog(parent),
    ui(new Ui::DialogTerrainPainting)
{
    ui->setupUi(this);

    // TEMP
    ui->brush_preview->hide();

    _terrain_original = terrain;
    _terrain_modified = new TerrainDefinition(NULL);

    ui->widget_commands->hide();

    revert();

    brushConfigChanged();
}

DialogTerrainPainting::~DialogTerrainPainting()
{
    delete ui;
}

void DialogTerrainPainting::keyReleaseEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_F2:
        ui->input_brush_mode->setCurrentIndex(0);
        event->accept();
        break;
    case Qt::Key_F3:
        ui->input_brush_mode->setCurrentIndex(1);
        event->accept();
        break;
    case Qt::Key_F4:
        ui->input_brush_mode->setCurrentIndex(2);
        event->accept();
        break;
    case Qt::Key_F12:
        ui->input_brush_mode->setCurrentIndex(3);
        event->accept();
        break;
    default:
        QDialog::keyReleaseEvent(event);
    }
}

void DialogTerrainPainting::wheelEvent(QWheelEvent* event)
{
    if (event->modifiers() & Qt::ControlModifier)
    {
        ui->input_brush_size->setValue(ui->input_brush_size->value() + (event->delta() > 0 ? 1 : -1));
        event->accept();
    }
    else if (event->modifiers() & Qt::ShiftModifier)
    {
        ui->input_brush_strength->setValue(ui->input_brush_strength->value() + (event->delta() > 0 ? 1 : -1));
        event->accept();
    }
    else if (event->modifiers() & Qt::AltModifier)
    {
        ui->input_brush_smoothing->setValue(ui->input_brush_smoothing->value() + (event->delta() > 0 ? 1 : -1));
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void DialogTerrainPainting::accept()
{
    _terrain_modified->copy(_terrain_original);
    QDialog::accept();
}

void DialogTerrainPainting::revert()
{
    _terrain_original->copy(_terrain_modified);

    ui->widget_heightmap->setTerrain(_terrain_modified);
    ui->widget_heightmap->setBrush(&_brush);
}

void DialogTerrainPainting::brushConfigChanged()
{
    // Fill brush object
    _brush.setMode((PaintingBrushMode) ui->input_brush_mode->currentIndex());
    _brush.setSize(ui->input_brush_size);
    _brush.setSmoothing(ui->input_brush_smoothing);
    _brush.setStrength(ui->input_brush_strength);

    // Update brush description
    ui->label_brush_description->setText(getHelpText());

    // Update brush preview
    // TODO

    // Tell to 3D widget
    ui->widget_heightmap->setBrush(&_brush);
}

void DialogTerrainPainting::heightmapChanged()
{
    unsigned long memused = _terrain_modified->getMemoryStats();
    ui->label_memory_consumption->setText(tr("Memory used: %1").arg(getHumanMemory(memused)));
    ui->progress_memory_consumption->setMaximum(1024);
    ui->progress_memory_consumption->setValue(memused / 1024);
}

QString DialogTerrainPainting::getHelpText()
{
    QString result = _brush.getHelpText();
    return result;
}
