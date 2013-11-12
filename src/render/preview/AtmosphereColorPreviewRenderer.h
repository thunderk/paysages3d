#ifndef ATMOSPHERECOLORPREVIEWRENDERER_H
#define ATMOSPHERECOLORPREVIEWRENDERER_H

#include "Base2dPreviewRenderer.h"

namespace paysages {
namespace preview {

class AtmosphereColorPreviewRenderer:public Base2dPreviewRenderer
{
public:
    AtmosphereColorPreviewRenderer();

    virtual Color getColor2D(double x, double y, double scaling) const override;

protected:
    double heading;
};

}
}

#endif // ATMOSPHERECOLORPREVIEWRENDERER_H
