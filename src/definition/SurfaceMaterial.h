#pragma once

#include "definition_global.h"

namespace paysages {
namespace definition {

class DEFINITIONSHARED_EXPORT SurfaceMaterial {
  public:
    SurfaceMaterial();
    SurfaceMaterial(const Color &color);
    SurfaceMaterial(const SurfaceMaterial &other);
    ~SurfaceMaterial();

    static const SurfaceMaterial &getDefault();

    void setColor(double r, double g, double b, double a);

    void save(PackStream *stream) const;
    void load(PackStream *stream);
    void copy(SurfaceMaterial *destination) const;
    void validate();

  public:
    Color *base;

    double ambient;
    double hardness;
    double reflection;
    double shininess;
};
}
}
