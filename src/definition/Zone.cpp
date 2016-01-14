#include "Zone.h"

#include "Curve.h"
#include "PackStream.h"
#include "Vector3.h"

Zone::Zone(DefinitionNode *parent) : DefinitionNode(parent, "zone", "zone") {
    value_by_height = new Curve;
    absolute_height = 1;
    value_by_height->setDefault(1.0);
    value_by_slope = new Curve;
    value_by_slope->setDefault(1.0);
}

Zone::~Zone() {
    delete value_by_height;
    delete value_by_slope;
}

void Zone::save(PackStream *stream) const {
    stream->write(&absolute_height);
    stream->write(&relative_height_min);
    stream->write(&relative_height_middle);
    stream->write(&relative_height_max);

    value_by_height->save(stream);
    value_by_slope->save(stream);
}

void Zone::load(PackStream *stream) {
    stream->read(&absolute_height);
    stream->read(&relative_height_min);
    stream->read(&relative_height_middle);
    stream->read(&relative_height_max);

    value_by_height->load(stream);
    value_by_slope->load(stream);
}

void Zone::copy(DefinitionNode *_destination) const {
    if (auto destination = static_cast<Zone *>(_destination)) {
        destination->absolute_height = absolute_height;
        destination->relative_height_min = relative_height_min;
        destination->relative_height_middle = relative_height_middle;
        destination->relative_height_max = relative_height_max;

        value_by_height->copy(destination->value_by_height);
        value_by_slope->copy(destination->value_by_slope);
    }
}

void Zone::clear() {
    value_by_height->clear();
    value_by_slope->clear();
}

void Zone::setAbsoluteHeight() {
    absolute_height = 1;
}

void Zone::setRelativeHeight(double min, double middle, double max) {
    if (max < min) {
        max = min;
    }
    if (middle < min) {
        middle = min;
    }
    if (middle > max) {
        middle = max;
    }

    absolute_height = 0;
    relative_height_min = min;
    relative_height_middle = middle;
    relative_height_max = max;
}

void Zone::getHeightCurve(Curve *curve) const {
    value_by_height->copy(curve);
}

void Zone::setHeightCurve(Curve *curve) {
    curve->copy(value_by_height);
}

void Zone::addHeightRangeQuick(double value, double hardmin, double softmin, double softmax, double hardmax) {
    value_by_height->addPoint(hardmin, 0.0);
    value_by_height->addPoint(softmin, value);
    value_by_height->addPoint(softmax, value);
    value_by_height->addPoint(hardmax, 0.0);
}

void Zone::getSlopeCurve(Curve *curve) const {
    value_by_slope->copy(curve);
}

void Zone::setSlopeCurve(Curve *curve) {
    curve->copy(value_by_slope);
}

void Zone::addSlopeRangeQuick(double value, double hardmin, double softmin, double softmax, double hardmax) {
    value_by_slope->addPoint(hardmin, 0.0);
    value_by_slope->addPoint(softmin, value);
    value_by_slope->addPoint(softmax, value);
    value_by_slope->addPoint(hardmax, 0.0);
}

double Zone::getValue(const Vector3 &location, const Vector3 &normal) const {
    double final_height;
    double value_height, value_steepness;

    if (absolute_height) {
        final_height = location.y;
    } else {
        if (location.y >= relative_height_max) {
            final_height = 1.0;
        } else if (location.y <= relative_height_min) {
            final_height = 0.0;
        } else if (location.y <= relative_height_middle) {
            final_height = 0.5 * (location.y - relative_height_min) / (relative_height_middle - relative_height_min);
        } else {
            final_height =
                0.5 + 0.5 * (location.y - relative_height_middle) / (relative_height_max - relative_height_middle);
        }
    }

    value_height = value_by_height->getValue(final_height);
    value_steepness = value_by_slope->getValue(1.0 - normal.y);

    if (value_steepness < value_height) {
        return value_steepness;
    } else {
        return value_height;
    }
}
