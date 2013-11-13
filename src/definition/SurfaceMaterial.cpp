#include "SurfaceMaterial.h"

#include "PackStream.h"

void materialSave(PackStream* stream, SurfaceMaterial* material)
{
    stream->write(&material->base.h);
    stream->write(&material->base.l);
    stream->write(&material->base.s);

    stream->write(&material->hardness);
    stream->write(&material->reflection);
    stream->write(&material->shininess);

    stream->write(&material->receive_shadows);
}

void materialLoad(PackStream* stream, SurfaceMaterial* material)
{
    stream->read(&material->base.h);
    stream->read(&material->base.l);
    stream->read(&material->base.s);

    stream->read(&material->hardness);
    stream->read(&material->reflection);
    stream->read(&material->shininess);

    stream->read(&material->receive_shadows);

    materialValidate(material);
}

void materialValidate(SurfaceMaterial* material)
{
    material->_rgb = colorFromHSL(material->base);
}
