#include "SurfaceMaterial.h"

#include "PackStream.h"

SurfaceMaterial::SurfaceMaterial():
    SurfaceMaterial(COLOR_BLACK)
{
}

SurfaceMaterial::SurfaceMaterial(const Color &color)
{
    base = colorToHSL(color);
    hardness = 0.5;
    reflection = 0.0;
    shininess = 0.0;
    receive_shadows = 1.0;
}

void SurfaceMaterial::save(PackStream* stream) const
{
    stream->write(&base.h);
    stream->write(&base.l);
    stream->write(&base.s);

    stream->write(&hardness);
    stream->write(&reflection);
    stream->write(&shininess);

    stream->write(&receive_shadows);
}

void SurfaceMaterial::load(PackStream* stream)
{
    stream->read(&base.h);
    stream->read(&base.l);
    stream->read(&base.s);

    stream->read(&hardness);
    stream->read(&reflection);
    stream->read(&shininess);

    stream->read(&receive_shadows);

    validate();
}

void SurfaceMaterial::validate()
{
    _rgb = colorFromHSL(base);
}
