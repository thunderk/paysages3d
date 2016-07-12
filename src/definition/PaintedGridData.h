#pragma once

#include "definition_global.h"

namespace paysages {
namespace definition {

/**
 * Internal storage class to hold data for a PaintedGrid.
 */
class PaintedGridData {
    friend class PaintedGrid;

  public:
    PaintedGridData();
    ~PaintedGridData();

    void copy(PaintedGridData *destination) const;
    void save(PackStream *stream) const;
    void load(PackStream *stream);

    /**
     * Clear all stored data.
     */
    void clear();

    inline bool hasData() const {
        return rows_count > 0;
    }

  private:
    typedef struct {
        int xstart;
        int xend;
        double *height;
    } HeightMapPixelGroup;

    typedef struct {
        int y;
        int pixel_groups_count;
        HeightMapPixelGroup *pixel_groups;
    } HeightMapRow;

    int memsize;
    int rows_count;
    HeightMapRow *rows;
};
}
}
