#include "SurfaceMaterial.h"

#include "PackStream.h"
#include "Color.h"

SurfaceMaterial::SurfaceMaterial():
    SurfaceMaterial(COLOR_BLACK)
{
}

SurfaceMaterial::SurfaceMaterial(const Color &color)
{
    base = new Color(color);
    hardness = 0.5;
    reflection = 0.0;
    shininess = 0.0;
    receive_shadows = 1.0;
}

SurfaceMaterial::~SurfaceMaterial()
{
    delete base;
}

void SurfaceMaterial::setColor(double r, double g, double b, double a)
{
    base->r = r;
    base->g = g;
    base->b = b;
    base->a = a;
}

void SurfaceMaterial::save(PackStream* stream) const
{
    base->save(stream);

    stream->write(&hardness);
    stream->write(&reflection);
    stream->write(&shininess);

    stream->write(&receive_shadows);
}

void SurfaceMaterial::load(PackStream* stream)
{
    base->load(stream);

    stream->read(&hardness);
    stream->read(&reflection);
    stream->read(&shininess);

    stream->read(&receive_shadows);
}

void SurfaceMaterial::validate()
{
}
