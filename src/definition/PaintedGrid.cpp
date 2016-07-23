#include "PaintedGrid.h"

#include "Interpolation.h"
#include "Memory.h"
#include "PaintedGridBrush.h"
#include "PaintedGridData.h"
#include <cassert>
#include <cmath>

PaintedGrid::PaintedGrid(DefinitionNode *parent) : DefinitionNode(parent, "grid", "grid") {
    merged_data = new PaintedGridData;
    brush_data = new PaintedGridData;
}

PaintedGrid::~PaintedGrid() {
    delete merged_data;
    delete brush_data;
}

void PaintedGrid::copy(DefinitionNode *_destination) const {
    PaintedGrid *destination = (PaintedGrid *)_destination;

    merged_data->copy(destination->merged_data);
    destination->brush_data->clear();
}

void PaintedGrid::save(PackStream *stream) const {
    merged_data->save(stream);
}

void PaintedGrid::load(PackStream *stream) {
    merged_data->load(stream);
    brush_data->clear();
}

bool PaintedGrid::getInterpolatedValue(double x, double y, double *result) const {
    int ix, iy;
    int xlow;
    int ylow;

    xlow = floor_to_int(x);
    ylow = floor_to_int(y);

    int hit = 0;
    for (ix = xlow - 1; ix <= xlow + 2 && !hit; ix++) {
        for (iy = ylow - 1; iy <= ylow + 2 && !hit; iy++) {
            if (getDataPointer(brush_data, x, y, NULL, false) || getDataPointer(merged_data, x, y, NULL, false)) {
                hit = 1;
            }
        }
    }

    if (hit && result) {
        double stencil[16];
        double value;
        for (ix = xlow - 1; ix <= xlow + 2; ix++) {
            for (iy = ylow - 1; iy <= ylow + 2; iy++) {
                if (!getGridValue(ix, iy, &value)) {
                    value = getInitialValue(ix, iy);
                }
                stencil[(iy - (ylow - 1)) * 4 + ix - (xlow - 1)] = value;
            }
        }

        *result = Interpolation::bicubic(stencil, x - to_double(xlow), y - to_double(ylow));
    }

    return hit;
}

bool PaintedGrid::getGridValue(int x, int y, double *result) const {
    double *dpointer;
    dpointer = getDataPointer(brush_data, x, y, NULL, false);
    if (dpointer) {
        *result = *dpointer;
        return true;
    } else {
        dpointer = getDataPointer(merged_data, x, y, NULL, false);
        if (dpointer) {
            *result = *dpointer;
            return true;
        } else {
            return false;
        }
    }
}

double PaintedGrid::getFinalValue(double x, double y) const {
    double result;

    if (getInterpolatedValue(x, y, &result)) {
        return result;
    } else {
        return getInitialValue(x, y);
    }
}

bool PaintedGrid::hasPainting() const {
    return merged_data->hasData() || brush_data->hasData();
}

bool PaintedGrid::isPainted(int x, int y) const {
    return getDataPointer(brush_data, x, y, NULL, false) || getDataPointer(merged_data, x, y, NULL, false);
}

unsigned long PaintedGrid::getMemoryStats() const {
    return merged_data->memsize + brush_data->memsize;
}

void PaintedGrid::clearPainting() {
    merged_data->clear();
    brush_data->clear();
}

void PaintedGrid::applyBrush(const PaintedGridBrush &brush, double x, double y, double force, bool commit) {
    int xstart, xend, ystart, yend;

    brush.getArea(x, y, &xstart, &ystart, &xend, &yend);

    int ix, iy;
    double dx, dy, influence;

    for (ix = xstart; ix <= xend; ix++) {
        dx = to_double(ix);
        for (iy = ystart; iy <= yend; iy++) {
            dy = to_double(iy);

            influence = brush.getInfluence(x - dx, y - dy);

            if (influence > 0.0) {
                double *dpointer = getDataPointer(brush_data, ix, iy, merged_data, true);
                *dpointer = brush.getValue(this, dx, dy, *dpointer, influence, force);
            }
        }
    }

    if (commit) {
        endBrushStroke();
    }
}

void PaintedGrid::endBrushStroke() {
    int i, j, k;
    PaintedGridData *data = brush_data;

    for (i = 0; i < data->rows_count; i++) {
        for (j = 0; j < data->rows[i].pixel_groups_count; j++) {
            for (k = 0; k < data->rows[i].pixel_groups[j].xend - data->rows[i].pixel_groups[j].xstart + 1; k++) {
                double *dpointer =
                    getDataPointer(merged_data, data->rows[i].pixel_groups[j].xstart + k, data->rows[i].y, NULL, true);
                *dpointer = data->rows[i].pixel_groups[j].height[k];
            }
        }
    }

    brush_data->clear();
}

double PaintedGrid::getInitialValue(double, double) const {
    return 0.0;
}

double *PaintedGrid::getDataPointer(PaintedGridData *data, int x, int y, PaintedGridData *fallback, bool grow) const {
    int i;

    /* Find row */
    /* TODO Dichotomic search */
    PaintedGridData::HeightMapRow *row;
    i = 0;
    while (i < data->rows_count && data->rows[i].y < y) {
        i++;
    }
    if (i < data->rows_count && data->rows[i].y == y) {
        row = data->rows + i;
    } else if (grow) {
        row = (PaintedGridData::HeightMapRow *)Memory::naiveArrayInsert(
            (void **)&data->rows, sizeof(PaintedGridData::HeightMapRow), data->rows_count, i);

        row->y = y;
        row->pixel_groups_count = 0;
        row->pixel_groups = (PaintedGridData::HeightMapPixelGroup *)malloc(1);

        data->rows_count++;
        data->memsize += sizeof(PaintedGridData::HeightMapRow);
    } else {
        return NULL;
    }

#ifndef NDEBUG
    /* Check rows */
    for (i = 1; i < data->rows_count; i++) {
        assert(data->rows[i].y > data->rows[i - 1].y);
    }
#endif

    /* Find pixel group */
    PaintedGridData::HeightMapPixelGroup *pixel_group = NULL;
    for (i = 0; i < row->pixel_groups_count; i++) {
        if (x < row->pixel_groups[i].xstart - 1) {
            break;
        } else if (x <= row->pixel_groups[i].xend + 1) {
            if (x == row->pixel_groups[i].xend + 1 && i < row->pixel_groups_count - 1 &&
                x == row->pixel_groups[i + 1].xstart) {
                /* Choose next group if it already includes the pixel */
                i++;
            }
            pixel_group = row->pixel_groups + i;
            break;
        }
    }

    /* Alter pixel group */
    double *pixel;
    int added = 1;
    if (!pixel_group) {
        if (!grow) {
            return NULL;
        }

        /* Create the pixel group with one pixel */
        pixel_group = (PaintedGridData::HeightMapPixelGroup *)Memory::naiveArrayInsert(
            (void **)&row->pixel_groups, sizeof(PaintedGridData::HeightMapPixelGroup), row->pixel_groups_count, i);

        pixel_group->xstart = x;
        pixel_group->xend = x;
        pixel_group->height = (double *)malloc(sizeof(double));

        pixel = pixel_group->height;

        row->pixel_groups_count++;
        data->memsize += sizeof(PaintedGridData::HeightMapPixelGroup) + sizeof(double);
    } else if (x == pixel_group->xstart - 1) {
        if (!grow) {
            return NULL;
        }

        /* Extend the rowgroup at start */
        pixel_group->xstart--;
        pixel = (double *)Memory::naiveArrayInsert((void **)&pixel_group->height, sizeof(double),
                                                   pixel_group->xend - pixel_group->xstart, 0);
        data->memsize += sizeof(double);
    } else if (x == pixel_group->xend + 1) {
        if (!grow) {
            return NULL;
        }

        /* Extend the rowgroup at end */
        pixel_group->xend++;
        pixel = (double *)Memory::naiveArrayInsert((void **)&pixel_group->height, sizeof(double),
                                                   pixel_group->xend - pixel_group->xstart,
                                                   pixel_group->xend - pixel_group->xstart);
        data->memsize += sizeof(double);
    } else {
        assert(x >= pixel_group->xstart);
        assert(x <= pixel_group->xend);
        pixel = pixel_group->height + x - pixel_group->xstart;
        added = 0;
    }

#ifndef NDEBUG
    /* Check pixel groups */
    for (i = 0; i < row->pixel_groups_count; i++) {
        if (i > 0) {
            assert(row->pixel_groups[i].xstart > row->pixel_groups[i - 1].xend);
        }
        if (i < row->pixel_groups_count - 1) {
            assert(row->pixel_groups[i].xend < row->pixel_groups[i + 1].xstart);
        }
        assert(row->pixel_groups[i].xend >= row->pixel_groups[i].xstart);
    }
#endif

    /* Reset pixel if it had been added */
    if (added) {
        if (fallback) {
            double *dpointer = getDataPointer(fallback, x, y, NULL, false);
            if (dpointer) {
                *pixel = *dpointer;
            } else {
                *pixel = getInitialValue(x, y);
            }
        } else {
            *pixel = getInitialValue(x, y);
        }
    }

    return pixel;
}
