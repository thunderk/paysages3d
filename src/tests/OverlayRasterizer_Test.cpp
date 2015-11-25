#include "BaseTestCase.h"
#include "OverlayRasterizer.h"

#include <vector>
#include "Scenery.h"
#include "SoftwareCanvasRenderer.h"
#include "Color.h"

typedef struct {
    int x;
    int y;
    double relx;
    double rely;
} PixelCall;

std::vector<PixelCall> calls;

class MockOverlayRasterizer : public OverlayRasterizer {
  public:
    MockOverlayRasterizer(SoftwareCanvasRenderer *renderer)
        : OverlayRasterizer(renderer, renderer->getProgressHelper()) {
    }

    virtual Color processPixel(int x, int y, double relx, double rely) const override {
        PixelCall call = {x, y, relx, rely};
        calls.push_back(call);
        return COLOR_BLUE;
    }
};

void checkCall(const PixelCall &call, int x, int y, double relx, double rely) {
    EXPECT_EQ(x, call.x);
    EXPECT_EQ(y, call.y);
    EXPECT_DOUBLE_EQ(relx, call.relx);
    EXPECT_DOUBLE_EQ(rely, call.rely);
}

TEST(OverlayRasterizer, pixelProcessing) {
    calls.clear();

    Scenery scenery;
    SoftwareCanvasRenderer renderer(&scenery);
    MockOverlayRasterizer rasterizer(&renderer);
    renderer.setSize(4, 3);
    renderer.setSoloRasterizer(&rasterizer);
    renderer.render();

    ASSERT_EQ(12, (int)calls.size());
    checkCall(calls[0], 0, 0, -1.5, -1.0);
    checkCall(calls[1], 0, 2, -1.5, 1.0);
    checkCall(calls[2], 2, 0, 0.5, -1.0);
    checkCall(calls[3], 2, 2, 0.5, 1.0);
    checkCall(calls[4], 0, 1, -1.5, 0.0);
    checkCall(calls[5], 1, 0, -0.5, -1.0);
    checkCall(calls[6], 1, 1, -0.5, 0.0);
    checkCall(calls[7], 1, 2, -0.5, 1.0);
    checkCall(calls[8], 2, 1, 0.5, 0.0);
    checkCall(calls[9], 3, 0, 1.5, -1.0);
    checkCall(calls[10], 3, 1, 1.5, 0.0);
    checkCall(calls[11], 3, 2, 1.5, 1.0);
}
