#ifndef ATMOSPHERECOLORPREVIEWRENDERER_H
#define ATMOSPHERECOLORPREVIEWRENDERER_H

#include "Base2dPreviewRenderer.h"

namespace paysages {
namespace preview {

class PREVIEWSHARED_EXPORT AtmosphereColorPreviewRenderer:public Base2dPreviewRenderer
{
public:
    AtmosphereColorPreviewRenderer(AtmosphereDefinition* definition, double heading);

    void bindEvent(BasePreview* preview) override;
    void updateEvent() override;
    virtual Color getColor2D(double x, double y, double scaling) override;

private:
    AtmosphereDefinition* definition;
    double heading;
};

}
}

#endif // ATMOSPHERECOLORPREVIEWRENDERER_H
