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
    SurfaceMaterial();
    SurfaceMaterial(const Color& color);

    void save(PackStream* stream) const;
    void load(PackStream* stream);

    void validate();

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

#endif // SURFACEMATERIAL_H
