#ifndef SURFACEMATERIAL_H
#define SURFACEMATERIAL_H

#include "definition_global.h"

namespace paysages {
namespace definition {

class DEFINITIONSHARED_EXPORT SurfaceMaterial
{
public:
    SurfaceMaterial();
    SurfaceMaterial(const Color& color);
    ~SurfaceMaterial();

    void setColor(double r, double g, double b, double a);

    void save(PackStream* stream) const;
    void load(PackStream* stream);
    void copy(SurfaceMaterial *destination) const;
    void validate();

public:
    Color *base;

    double hardness;
    double reflection;
    double shininess;

    double receive_shadows;
};

}
}

#endif // SURFACEMATERIAL_H
