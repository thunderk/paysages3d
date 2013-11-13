#ifndef SURFACEMATERIAL_H
#define SURFACEMATERIAL_H

#include "definition_global.h"

// TODO Change to pointers and forward declaration
#include "ColorHSL.h"
#include "Color.h"

namespace paysages {
namespace definition {

class DEFINITIONSHARED_EXPORT SurfaceMaterial
{
public:
    ColorHSL base;

    double hardness;
    double reflection;
    double shininess;

    double receive_shadows;

    Color _rgb;
};

}
}

DEFINITIONSHARED_EXPORT void materialSave(PackStream* stream, SurfaceMaterial* material);
DEFINITIONSHARED_EXPORT void materialLoad(PackStream* stream, SurfaceMaterial* material);
DEFINITIONSHARED_EXPORT void materialValidate(SurfaceMaterial* material);

#endif // SURFACEMATERIAL_H
