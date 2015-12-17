#include "ColorProfile.h"

#include "PackStream.h"
#include <cmath>

ColorProfile::ColorProfile() {
    setToneMapping(TONE_MAPPING_UNCHARTED, 1.6);
}

ColorProfile::ColorProfile(ToneMappingOperator tonemapper, double exposure) {
    setToneMapping(tonemapper, exposure);
}

static inline double _uncharted2Tonemap(double x) {
    double A = 0.15;
    double B = 0.50;
    double C = 0.10;
    double D = 0.20;
    double E = 0.02;
    double F = 0.30;

    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

static inline Color _toneMappingUncharted(const Color &pixel, double exposure) {
    static const double W = 11.2;
    static const double white_scale = 1.0 / _uncharted2Tonemap(W);
    static const double factor = 1.0 / 2.2;

    Color result(pow(_uncharted2Tonemap(pixel.r * exposure) * white_scale, factor),
                 pow(_uncharted2Tonemap(pixel.g * exposure) * white_scale, factor),
                 pow(_uncharted2Tonemap(pixel.b * exposure) * white_scale, factor), pixel.a);

    return result;
}

static inline Color _toneMappingReinhard(const Color &pixel, double exposure) {
    Color result((pixel.r * exposure) / (1.0 + pixel.r * exposure), (pixel.g * exposure) / (1.0 + pixel.g * exposure),
                 (pixel.b * exposure) / (1.0 + pixel.b * exposure), pixel.a);

    return result;
}

static inline Color _toneMappingClamp(const Color &pixel) {
    Color result(pixel.r > 1.0 ? 1.0 : pixel.r, pixel.g > 1.0 ? 1.0 : pixel.g, pixel.b > 1.0 ? 1.0 : pixel.b, pixel.a);

    return result;
}

void ColorProfile::setToneMapping(ToneMappingOperator tonemapper, double exposure) {
    this->mapper = tonemapper;
    this->exposure = exposure;
}

void ColorProfile::save(PackStream *stream) const {
    int imapper = trunc_to_int(mapper);
    stream->write(&imapper);
    stream->write(&exposure);
}

void ColorProfile::load(PackStream *stream) {
    int imapper = trunc_to_int(mapper);
    stream->read(&imapper);
    stream->read(&exposure);

    mapper = (ToneMappingOperator)imapper;
}

void ColorProfile::copy(ColorProfile *destination) const {
    destination->mapper = mapper;
    destination->exposure = exposure;
}

Color ColorProfile::apply(const Color &pixel) const {
    switch (mapper) {
    case TONE_MAPPING_REIHNARD:
        return _toneMappingReinhard(pixel, exposure);
    case TONE_MAPPING_UNCHARTED:
        return _toneMappingUncharted(pixel, exposure);
    default:
        return _toneMappingClamp(pixel);
    }
}
