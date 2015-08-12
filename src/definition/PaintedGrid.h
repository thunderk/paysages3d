#ifndef PAINTEDGRID_H
#define PAINTEDGRID_H

#include "definition_global.h"

#include "DefinitionNode.h"

namespace paysages {
namespace definition {

/**
 * Grid of values that can be painted using a brush.
 *
 * This may be useful for terrain altitude, texture presence...
 *
 * Grid cells are considered to be 1.0-sized.
 */
class DEFINITIONSHARED_EXPORT PaintedGrid: public DefinitionNode
{
public:
    PaintedGrid(DefinitionNode *parent=0);
    virtual ~PaintedGrid();

    virtual void copy(DefinitionNode *destination) const override;
    virtual void save(PackStream *stream) const override;
    virtual void load(PackStream *stream) override;

    /**
     * Get the interpolated value at a given location.
     *
     * Returns true if the value is user-specified in the grid, false otherwise.
     */
    bool getInterpolatedValue(double x, double y, double *result) const;

    /**
     * Get the grid-stored value at a grid point.
     *
     * Returns true if the value is user-specified in the grid, false otherwise.
     */
    bool getGridValue(int x, int y, double *result) const;

    /**
     * Get the final interpolated value, using initial value as fallback.
     */
    double getFinalValue(double x, double y) const;

    /**
     * Returns true if a grid point is user-specified, false if it's the initial value.
     */
    bool isPainted(int x, int y) const;

    /**
     * Get the number of bytes used by this grid.
     */
    unsigned long getMemoryStats() const;

    /**
     * Clear all painting and reset the initial value everywhere.
     */
    void clearPainting();

    /**
     * Apply a brush stroke at a grid location (locating the brush center).
     */
    void applyBrush(const PaintedGridBrush &brush, double x, double y, double force);

    /**
     * Commit previous brush strokes.
     */
    void endBrushStroke();

    /**
     * Virtual method that can be reimplemented to provide the initial value at a grid location.
     *
     * By default, the initial value will be 0.0.
     */
    virtual double getInitialValue(double x, double y) const;

private:
    double *getDataPointer(PaintedGridData *data, int x, int y, PaintedGridData *fallback, bool grow) const;

private:
    PaintedGridData *merged_data;
    PaintedGridData *brush_data;
};

}
}

#endif // PAINTEDGRID_H
