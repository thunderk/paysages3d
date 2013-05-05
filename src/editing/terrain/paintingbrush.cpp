#include "paintingbrush.h"

#include <cmath>

PaintingBrush::PaintingBrush()
{
    _mode = PAINTING_BRUSH_RAISE;
    _size = 0.0;
    _smoothing = 0.0;
    _strength = 0.0;
    _noise = noiseCreateGenerator();
    noiseAddLevelsSimple(_noise, 10, 1.0, -0.5, 0.5, 0.5);
}

PaintingBrush::~PaintingBrush()
{
    noiseDeleteGenerator(_noise);
}

void PaintingBrush::setMode(PaintingBrushMode mode)
{
    _mode = mode;
}

void PaintingBrush::setSize(double value)
{
    _size = value;
}

void PaintingBrush::setSize(QAbstractSlider* slider)
{
    setSize(20.0 * (double)slider->value() / (double)slider->maximum());
}

void PaintingBrush::setSmoothing(double value)
{
    _smoothing = value;
}

void PaintingBrush::setSmoothing(QAbstractSlider* slider)
{
    setSmoothing((double)slider->value() / (double)slider->maximum());
}

void PaintingBrush::setStrength(double value)
{
    _strength = value;
}

void PaintingBrush::setStrength(QAbstractSlider* slider)
{
    setStrength((double)slider->value() / (double)slider->maximum());
}

void PaintingBrush::randomizeNoise()
{
    noiseRandomizeOffsets(_noise);
}

double PaintingBrush::getInfluence(double relative_x, double relative_z)
{
    double distance = sqrt(relative_x * relative_x + relative_z * relative_z);
    if (distance > _size)
    {
        return 0.0;
    }
    else if (distance > _size * (1.0 - _smoothing))
    {
        return 1.0 - (distance - _size * (1.0 - _smoothing)) / (_size * _smoothing);
    }
    else
    {
        return 1.0;
    }
}

void PaintingBrush::drawPreview(QWidget* widget)
{

}

void PaintingBrush::applyToTerrain(TerrainDefinition* terrain, double x, double z, double duration, bool reverse)
{
    double brush_strength;
    TerrainBrush brush;

    brush.relative_x = x;
    brush.relative_z = z;
    brush.hard_radius = _size * (1.0 - _smoothing);
    brush.smoothed_size = _size * _smoothing;
    brush.total_radius = brush.hard_radius + brush.smoothed_size;

    brush_strength = 0.5 * _strength * duration / 0.1;

    switch (_mode)
    {
    case PAINTING_BRUSH_RAISE:
        if (reverse)
        {
            brush_strength = -brush_strength;
        }
        terrainBrushElevation(terrain->height_map, &brush, brush_strength * 2.0);
        break;
    case PAINTING_BRUSH_SMOOTH:
        if (reverse)
        {
            terrainBrushSmooth(terrain->height_map, &brush, brush_strength);
        }
        else
        {
            terrainBrushAddNoise(terrain->height_map, &brush, _noise, brush_strength * 0.5);
        }
        break;
    case PAINTING_BRUSH_RESTORE:
        terrainBrushReset(terrain->height_map, &brush, brush_strength);
        break;
    default:
        return;
    }
}
