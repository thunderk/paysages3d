#include "BaseTestCase.h"
#include "Rasterizer.h"

#include "SoftwareRenderer.h"
#include "Scenery.h"
#include "Vector3.h"
#include "CameraDefinition.h"
#include "Color.h"
#include "CanvasPortion.h"

class FakeRasterizer: public Rasterizer
{
public:
    FakeRasterizer(SoftwareRenderer *renderer): Rasterizer(renderer, NULL, 0, COLOR_WHITE)
    {
    }
    virtual Color shadeFragment(const CanvasFragment &) const override
    {
        return COLOR_RED;
    }
    virtual int prepareRasterization() override
    {
        return 0;
    }
    virtual void rasterizeToCanvas(CanvasPortion *) override
    {
    }
};

TEST(Rasterizer, autoSplitNearFrustum)
{
    Scenery scenery;
    scenery.getCamera()->setLocation(Vector3(0.0, 5.0, 0.0));
    scenery.getCamera()->setTarget(Vector3(0.0, 5.0, 1.0));
    SoftwareRenderer renderer(&scenery);

    FakeRasterizer rast(&renderer);
    CanvasPortion portion;
    portion.setSize(300, 300);
    portion.preparePixels();

    rast.pushTriangle(&portion, Vector3(0.0, 0.0, 8.0), Vector3(0.0, 0.0, 10.0), Vector3(2.0, 0.0, 8.0));
    EXPECT_EQ(1, rast.getTriangleCount());

    rast.resetTriangleCount();
    rast.setAutoCutLimit(15.0);
    rast.pushTriangle(&portion, Vector3(0.0, 0.0, 0.0), Vector3(-10.0, 0.0, 10.0), Vector3(10.0, 0.0, 10.0));
    EXPECT_EQ(0, rast.getTriangleCount());

    rast.resetTriangleCount();
    rast.setAutoCutLimit(9.0);
    rast.pushTriangle(&portion, Vector3(0.0, 0.0, 0.0), Vector3(-10.0, 0.0, 10.0), Vector3(10.0, 0.0, 10.0));
    EXPECT_EQ(3, rast.getTriangleCount());
}
