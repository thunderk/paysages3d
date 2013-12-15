#ifndef SCENERYTOPDOWNPREVIEWRENDERER_H
#define SCENERYTOPDOWNPREVIEWRENDERER_H

#include "preview_global.h"

#include "Base2dPreviewRenderer.h"

namespace paysages {
namespace preview {

class PREVIEWSHARED_EXPORT SceneryTopDownPreviewRenderer : public Base2dPreviewRenderer
{
public:
    SceneryTopDownPreviewRenderer(Scenery* scenery);

protected:
    virtual void bindEvent(BasePreview* preview) override;
    virtual void updateEvent() override;
    virtual Color getColor2D(double x, double y, double scaling) override;

    virtual void toggleChangeEvent(const std::string &key, bool value) override;

    virtual double getPrecision(const Vector3 &location) override;
    virtual Vector3 getCameraLocation(const Vector3 &target) override;
    virtual Color applyMediumTraversal(Vector3 location, Color color) override;

private:
    Scenery* scenery;
    bool clouds_enabled;
    bool specularity_enabled;
    CloudsDefinition* no_clouds;
};

}
}

#endif // SCENERYTOPDOWNPREVIEWRENDERER_H
