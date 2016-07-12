#pragma once

#include "definition_global.h"

namespace paysages {
namespace definition {

/**
 * Base class for brushes that can be used to paint values on a PaintedGrid.
 */
class DEFINITIONSHARED_EXPORT PaintedGridBrush {
  public:
    PaintedGridBrush(double hard_radius, double smoothed_size, double total_radius);

    /**
     * Get the rectangle area potentially covered by this brush.
     */
    void getArea(double x, double y, int *xstart, int *ystart, int *xend, int *yend) const;

    /**
     * Get the brush influence for coordinates relative to its center.
     */
    double getInfluence(double dx, double dy) const;

    /**
     * Abstract method to reimplement to get the final value of a brush stroke at a given point.
     */
    virtual double getValue(const PaintedGrid *grid, double x, double y, double basevalue, double influence,
                            double force) const;

  protected:
    double hard_radius;
    double smoothed_size;
    double total_radius;
};

/**
 * Brush able to raise or lower the grid value.
 */
class DEFINITIONSHARED_EXPORT PaintedGridBrushRaiseLower : public PaintedGridBrush {
  public:
    PaintedGridBrushRaiseLower(const PaintedGridBrush &brush) : PaintedGridBrush(brush) {
    }
    double getValue(const PaintedGrid *grid, double x, double z, double basevalue, double influence,
                    double force) const override;
};

/**
 * Brush able to smooth the value in an area.
 */
class DEFINITIONSHARED_EXPORT PaintedGridBrushSmooth : public PaintedGridBrush {
  public:
    PaintedGridBrushSmooth(const PaintedGridBrush &brush) : PaintedGridBrush(brush) {
    }
    double getValue(const PaintedGrid *grid, double x, double z, double basevalue, double influence,
                    double force) const override;
};

/**
 * Brush able to add random fractal noise.
 */
class DEFINITIONSHARED_EXPORT PaintedGridBrushAddNoise : public PaintedGridBrush {
  public:
    PaintedGridBrushAddNoise(const PaintedGridBrush &brush, NoiseGenerator *generator)
        : PaintedGridBrush(brush), generator(generator) {
    }
    double getValue(const PaintedGrid *grid, double x, double z, double basevalue, double influence,
                    double force) const override;

  private:
    NoiseGenerator *generator;
};

/**
 * Brush able to reset to initial value.
 */
class DEFINITIONSHARED_EXPORT PaintedGridBrushReset : public PaintedGridBrush {
  public:
    PaintedGridBrushReset(const PaintedGridBrush &brush) : PaintedGridBrush(brush) {
    }
    double getValue(const PaintedGrid *grid, double x, double z, double basevalue, double influence,
                    double force) const override;
};

/**
 * Brush able to flatten to a specific value.
 */
class DEFINITIONSHARED_EXPORT PaintedGridBrushFlatten : public PaintedGridBrush {
  public:
    PaintedGridBrushFlatten(const PaintedGridBrush &brush, double target) : PaintedGridBrush(brush), target(target) {
    }
    double getValue(const PaintedGrid *grid, double x, double z, double basevalue, double influence,
                    double force) const override;

  private:
    double target;
};
}
}
