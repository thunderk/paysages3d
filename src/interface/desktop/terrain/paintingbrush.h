#ifndef PAINTINGBRUSH_H
#define PAINTINGBRUSH_H

#include "desktop_global.h"

class QAbstractSlider;
class QWidget;

typedef enum
{
    PAINTING_BRUSH_RAISE = 0,
    PAINTING_BRUSH_SMOOTH = 1,
    PAINTING_BRUSH_FLATTEN = 2,
    PAINTING_BRUSH_RESTORE = 3
} PaintingBrushMode;

class PaintingBrush
{
public:
    PaintingBrush();
    ~PaintingBrush();

    void setMode(PaintingBrushMode mode);
    void setSize(double value);
    void setSize(QAbstractSlider* slider);
    void setSmoothing(double value);
    void setSmoothing(QAbstractSlider* slider);
    void setStrength(double value);
    void setStrength(QAbstractSlider* slider);
    void randomizeNoise();

    double getInfluence(double relative_x, double relative_z);

    void drawPreview(QWidget* widget);
    QString getHelpText();

    void applyToTerrain(TerrainDefinition* terrain, double x, double z, double duration, bool reverse);

private:
    PaintingBrushMode _mode;
    double _size;
    double _smoothing;
    double _strength;
    double _height;
    NoiseGenerator* _noise;
};

#endif // PAINTINGBRUSH_H
