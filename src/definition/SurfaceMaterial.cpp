#include "SurfaceMaterial.h"

#include "PackStream.h"
#include "Color.h"

static SurfaceMaterial DEFAULT;

SurfaceMaterial::SurfaceMaterial() : SurfaceMaterial(COLOR_BLACK) {
}

SurfaceMaterial::SurfaceMaterial(const Color &color) {
    base = new Color(color);
    ambient = 0.0;
    hardness = 0.5;
    reflection = 0.0;
    shininess = 0.0;
}

SurfaceMaterial::SurfaceMaterial(const SurfaceMaterial &other) : SurfaceMaterial(COLOR_BLACK) {
    other.copy(this);
}

SurfaceMaterial::~SurfaceMaterial() {
    delete base;
}

const SurfaceMaterial &SurfaceMaterial::getDefault() {
    return DEFAULT;
}

void SurfaceMaterial::setColor(double r, double g, double b, double a) {
    base->r = r;
    base->g = g;
    base->b = b;
    base->a = a;
}

void SurfaceMaterial::save(PackStream *stream) const {
    base->save(stream);

    stream->write(&ambient);
    stream->write(&hardness);
    stream->write(&reflection);
    stream->write(&shininess);
}

void SurfaceMaterial::load(PackStream *stream) {
    base->load(stream);

    stream->read(&ambient);
    stream->read(&hardness);
    stream->read(&reflection);
    stream->read(&shininess);
}

void SurfaceMaterial::copy(SurfaceMaterial *destination) const {
    *destination->base = *base;
    destination->ambient = ambient;
    destination->hardness = hardness;
    destination->reflection = reflection;
    destination->shininess = shininess;
    destination->validate();
}

void SurfaceMaterial::validate() {
}
