#include "FractalNoise.h"

#include <cassert>
#include <cmath>
#include <sstream>
#include "PackStream.h"
#include "Vector3.h"
#include "RandomGenerator.h"

FractalNoise::FractalNoise() {
    scaling = 1.0;
    height = 1.0;
    step_scaling = 2.0;
    step_height = 0.5;
}

FractalNoise::~FractalNoise() {
}

void FractalNoise::save(PackStream *stream) const {
    stream->write(&scaling);
    stream->write(&height);
    stream->write(&step_scaling);
    stream->write(&step_height);
    state.save(stream);
}

void FractalNoise::load(PackStream *stream) {
    stream->read(&scaling);
    stream->read(&height);
    stream->read(&step_scaling);
    stream->read(&step_height);
    state.load(stream);
}

void FractalNoise::copy(FractalNoise *destination) const {
    destination->scaling = scaling;
    destination->height = height;
    destination->step_scaling = step_scaling;
    destination->step_height = step_height;
    state.copy(&destination->state);
}

void FractalNoise::randomize(RandomGenerator &random) {
    state.randomizeOffsets(random);
}

void FractalNoise::setScaling(double scaling, double height) {
    this->scaling = scaling < 0.00000001 ? 0.0 : 1.0 / scaling;
    this->height = scaling * height;
}

void FractalNoise::setStep(double scaling_factor, double height_factor) {
    this->step_scaling = scaling_factor < 0.00000001 ? 0.0 : 1.0 / scaling_factor;
    this->step_height = scaling_factor * height_factor;

    // Ensure height will converge to 0
    if (this->step_height >= 0.99) {
        this->step_height = 0.99;
    }
}

void FractalNoise::setState(const NoiseState &state) {
    state.copy(&this->state);
}

double FractalNoise::get1d(double detail, double x) const {
    double current_scaling = scaling;
    double current_height = height;
    double result = 0.0;
    auto state_level_count = state.level_offsets.size();
    decltype(state_level_count) i = 0;

    while (current_height >= detail) {
        auto offset = state.level_offsets[i];

        result += getBase1d(offset.x + x * current_scaling) * current_height;

        current_scaling *= step_scaling;
        current_height *= step_height;

        i++;
        if (i >= state_level_count) {
            i = 0;
        }
    }

    return result;
}

double FractalNoise::get2d(double detail, double x, double y) const {
    double current_scaling = scaling;
    double current_height = height;
    double result = 0.0;
    auto state_level_count = state.level_offsets.size();
    decltype(state_level_count) i = 0;

    while (current_height >= detail) {
        auto offset = state.level_offsets[i];

        result += getBase2d(offset.x + x * current_scaling, offset.y + y * current_scaling) * current_height;

        current_scaling *= step_scaling;
        current_height *= step_height;

        i++;
        if (i >= state_level_count) {
            i = 0;
        }
    }

    return result;
}

double FractalNoise::get3d(double detail, double x, double y, double z) const {
    double current_scaling = scaling;
    double current_height = height;
    double result = 0.0;
    auto state_level_count = state.level_offsets.size();
    decltype(state_level_count) i = 0;

    while (current_height >= detail) {
        auto offset = state.level_offsets[i];

        result +=
            getBase3d(offset.x + x * current_scaling, offset.y + y * current_scaling, offset.z + z * current_scaling) *
            current_height;

        current_scaling *= step_scaling;
        current_height *= step_height;

        i++;
        if (i >= state_level_count) {
            i = 0;
        }
    }

    return result;
}

double FractalNoise::getTriplanar(double detail, const Vector3 &location, const Vector3 &normal) const {
    double noiseXY = get2d(detail, location.x, location.y);
    double noiseXZ = get2d(detail, location.x, location.z);
    double noiseYZ = get2d(detail, location.y, location.z);

    double mXY = fabs(normal.z);
    double mXZ = fabs(normal.y);
    double mYZ = fabs(normal.x);
    double total = 1.0 / (mXY + mXZ + mYZ);
    mXY *= total;
    mXZ *= total;
    mYZ *= total;

    return noiseXY * mXY + noiseXZ * mXZ + noiseYZ * mYZ;
}

void FractalNoise::estimateRange(double *min, double *max, double detail) const {
    *min = 0.0;
    *max = 0.0;

    double current_height = height;
    while (current_height >= detail) {
        *min += -0.5 * current_height;
        *max += 0.5 * current_height;
        current_height *= step_height;
    }
}

double FractalNoise::getBase1d(double x) const {
    return getBase2d(x, 0.0);
}

double FractalNoise::getBase2d(double x, double y) const {
    return getBase3d(x, y, 0.0);
}

string FractalNoise::checkDistribution() {
    stringstream result;

    double val, min, max, mean;
    RandomGenerator random;

    int samples = 10000000;
    double factor = 1.0 / to_double(samples);

    min = 0.0;
    max = 0.0;
    mean = 0.0;
    for (int i = 0; i < samples; i++) {
        val = getBase1d((random.genDouble() - 0.5) * 10.0);
        min = std::min(val, min);
        max = std::max(val, max);
        mean += val * factor;
    }
    result << "1d : min=" << min << " max=" << max << " mean=" << mean << endl;

    min = 0.0;
    max = 0.0;
    mean = 0.0;
    for (int i = 0; i < samples; i++) {
        val = getBase2d((random.genDouble() - 0.5) * 10.0, (random.genDouble() - 0.5) * 10.0);
        min = std::min(val, min);
        max = std::max(val, max);
        mean += val * factor;
    }
    result << "2d : min=" << min << " max=" << max << " mean=" << mean << endl;

    min = 0.0;
    max = 0.0;
    mean = 0.0;
    for (int i = 0; i < samples; i++) {
        val = getBase3d((random.genDouble() - 0.5) * 10.0, (random.genDouble() - 0.5) * 10.0,
                        (random.genDouble() - 0.5) * 10.0);
        min = std::min(val, min);
        max = std::max(val, max);
        mean += val * factor;
    }
    result << "3d : min=" << min << " max=" << max << " mean=" << mean << endl;

    return result.str();
}
