#include "PaintedGridBrush.h"

#include "NoiseGenerator.h"
#include "PaintedGrid.h"
#include <cmath>

PaintedGridBrush::PaintedGridBrush(double hard_radius, double smoothed_size, double total_radius)
    : hard_radius(hard_radius), smoothed_size(smoothed_size), total_radius(total_radius) {
}

void PaintedGridBrush::getArea(double x, double y, int *xstart, int *ystart, int *xend, int *yend) const {
    double s = smoothed_size + hard_radius;

    *xstart = floor_to_int(x - s);
    *xend = ceil_to_int(x + s);
    *ystart = floor_to_int(y - s);
    *yend = ceil_to_int(y + s);
}

double PaintedGridBrush::getInfluence(double dx, double dy) const {
    double distance = sqrt(dx * dx + dy * dy);

    if (distance > hard_radius) {
        if (distance <= hard_radius + smoothed_size) {
            return 1.0 - (distance - hard_radius) / smoothed_size;
        } else {
            return 0.0;
        }
    } else {
        return 1.0;
    }
}

double PaintedGridBrush::getValue(const PaintedGrid *, double, double, double basevalue, double, double) const {
    return basevalue;
}

double PaintedGridBrushRaiseLower::getValue(const PaintedGrid *, double, double, double basevalue, double influence,
                                            double force) const {
    return basevalue + influence * force;
}

double PaintedGridBrushSmooth::getValue(const PaintedGrid *grid, double x, double y, double basevalue, double influence,
                                        double force) const {
    double ideal, factor;
    ideal = grid->getFinalValue((x + total_radius * 0.5), y);
    ideal += grid->getFinalValue((x - total_radius * 0.5), y);
    ideal += grid->getFinalValue(x, (y - total_radius * 0.5));
    ideal += grid->getFinalValue(x, (y + total_radius * 0.5));
    ideal /= 4.0;
    factor = influence * force;
    if (factor > 1.0) {
        factor = 0.0;
    }
    return basevalue + (ideal - basevalue) * factor;
}

double PaintedGridBrushAddNoise::getValue(const PaintedGrid *, double x, double y, double basevalue, double influence,
                                          double force) const {
    return basevalue + generator->get2DTotal(x / total_radius, y / total_radius) * influence * force * total_radius;
}

double PaintedGridBrushReset::getValue(const PaintedGrid *grid, double x, double y, double basevalue, double influence,
                                       double force) const {
    double ideal = grid->getInitialValue(x, y);
    return basevalue + (ideal - basevalue) * influence * force;
}

double PaintedGridBrushFlatten::getValue(const PaintedGrid *, double, double, double basevalue, double influence,
                                         double force) const {
    double ideal = target;
    return basevalue + (ideal - basevalue) * influence * force;
}
