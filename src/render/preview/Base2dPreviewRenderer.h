#ifndef BASE2DPREVIEWRENDERER_H
#define BASE2DPREVIEWRENDERER_H

#include "preview_global.h"

#include "tools/color.h"

namespace paysages {
namespace preview {

class PREVIEWSHARED_EXPORT Base2dPreviewRenderer
{

public:
    Base2dPreviewRenderer();

    virtual void updateEvent();
    virtual Color getColor2D(double x, double y, double scaling) const;

protected:
    SoftwareRenderer* renderer;
};

}
}

#endif // BASE2DPREVIEWRENDERER_H
