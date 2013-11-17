#include "paintingbrush.h"

#include <cmath>
#include <QAbstractSlider>
#include "NoiseGenerator.h"
#include "TerrainDefinition.h"
#include "TerrainHeightMap.h"
#include "TerrainHeightMapBrush.h"

PaintingBrush::PaintingBrush()
{
    _mode = PAINTING_BRUSH_RAISE;
    _size = 0.0;
    _smoothing = 0.0;
    _strength = 0.0;
    _height = 0.0;
    _noise = new NoiseGenerator();
    _noise->addLevelsSimple(10, 1.0, -0.5, 0.5, 0.5);
}

PaintingBrush::~PaintingBrush()
{
    delete _noise;
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
    double value = (double)slider->value() / (double)slider->maximum();
    setSize(2.0 + value * value * 28.0);
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
    setStrength(0.1 + 0.9 * (double)slider->value() / (double)slider->maximum());
}

void PaintingBrush::randomizeNoise()
{
    _noise->randomizeOffsets();
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

void PaintingBrush::drawPreview(QWidget*)
{
}

QString PaintingBrush::getHelpText()
{
    switch (_mode)
    {
    case PAINTING_BRUSH_RAISE:
        return QObject::tr("<strong>Left click</strong>: raise terrain<br><br><strong>Right click</strong>: lower terrain");
    case PAINTING_BRUSH_SMOOTH:
        return QObject::tr("<strong>Left click</strong>: add random noise to terrain<br><br><strong>Right click</strong>: smooth details");
    case PAINTING_BRUSH_FLATTEN:
        return QObject::tr("<strong>Left click</strong>: flatten at height picked with right click<br><br><strong>Right click</strong>: pick height at center");
    case PAINTING_BRUSH_RESTORE:
        return QObject::tr("<strong>Left click</strong>: remove your manual modifications from terrain");
    }
    return QString("");
}

void PaintingBrush::applyToTerrain(TerrainDefinition* terrain, double x, double z, double duration, bool reverse)
{
    double brush_strength;
    TerrainHeightMapBrush brush;

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
        terrain->height_map->brushElevation(brush, brush_strength * 2.0);
        break;
    case PAINTING_BRUSH_SMOOTH:
        if (reverse)
        {
            terrain->height_map->brushSmooth(brush, brush_strength * 30.0);
        }
        else
        {
            terrain->height_map->brushAddNoise(brush, _noise, brush_strength * 0.3);
        }
        break;
    case PAINTING_BRUSH_FLATTEN:
        if (reverse)
        {
            _height = terrain->getInterpolatedHeight(x * terrain->scaling, z * terrain->scaling, 0, 1);
        }
        else
        {
            terrain->height_map->brushFlatten(brush, _height, brush_strength * 30.0);
        }
        break;
    case PAINTING_BRUSH_RESTORE:
        terrain->height_map->brushReset(brush, brush_strength * 30.0);
        break;
    default:
        return;
    }
}
