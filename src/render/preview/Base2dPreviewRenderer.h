#ifndef BASE2DPREVIEWRENDERER_H
#define BASE2DPREVIEWRENDERER_H

#include "preview_global.h"

#include "SoftwareRenderer.h"

#include "Color.h"

namespace paysages {
namespace preview {

class PREVIEWSHARED_EXPORT Base2dPreviewRenderer: protected SoftwareRenderer
{

public:
    Base2dPreviewRenderer();
    virtual ~Base2dPreviewRenderer();

    virtual void bindEvent(BasePreview* preview);

    virtual void toggleChangeEvent(const std::string &key, bool value);
    virtual void choiceChangeEvent(const std::string &key, int position);

    virtual void updateEvent();
    virtual void cameraEvent(double x, double y, double scaling);
    virtual Color getColor2D(double x, double y, double scaling);
};

}
}

#endif // BASE2DPREVIEWRENDERER_H
