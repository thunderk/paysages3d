#include "BaseTestCase.h"

#include "CanvasPixel.h"

TEST(CanvasPixel, MaxFragments) {
    CanvasPixel pixel;

    // One opaque fragment
    pixel.pushFragment(CanvasFragment(true, Vector3(0.0, 0.0, 0.0), Vector3(), 0, true));

    // Overflow max fragment count with transparent fragments
    for (int i = 0; i < MAX_FRAGMENTS_PER_PIXEL * 2; i++) {
        pixel.pushFragment(CanvasFragment(true, Vector3(0.0, 0.0, (double)i), Vector3(), 0, false));
    }

    ASSERT_EQ(MAX_FRAGMENTS_PER_PIXEL, pixel.getFragmentCount());
    EXPECT_DOUBLE_EQ(pixel.getFragment(0).getZ(), 0.0);
    EXPECT_EQ(pixel.getFragment(0).getOpaque(), true);
    EXPECT_DOUBLE_EQ(pixel.getFragment(MAX_FRAGMENTS_PER_PIXEL - 1).getZ(), (double)(MAX_FRAGMENTS_PER_PIXEL * 2 - 1));
    EXPECT_EQ(pixel.getFragment(MAX_FRAGMENTS_PER_PIXEL - 1).getOpaque(), false);
}

TEST(CanvasPixel, SameTransparent) {
    CanvasPixel pixel;

    pixel.pushFragment(CanvasFragment(true, Vector3(0.0, 0.0, 1.4), Vector3(), 0, false));
    pixel.pushFragment(CanvasFragment(true, Vector3(0.0, 0.0, 1.4), Vector3(), 0, false));

    ASSERT_EQ(1, pixel.getFragmentCount());

    pixel.pushFragment(CanvasFragment(true, Vector3(0.0, 0.0, 1.4), Vector3(), 1, false));

    ASSERT_EQ(2, pixel.getFragmentCount());

    EXPECT_EQ(pixel.getFragment(0).getClient(), 0);
    EXPECT_EQ(pixel.getFragment(1).getClient(), 1);
}

TEST(CanvasPixel, SameOpaque) {
    CanvasPixel pixel;

    pixel.pushFragment(CanvasFragment(true, Vector3(0.0, 0.0, 1.4), Vector3(), 0, true));
    pixel.pushFragment(CanvasFragment(true, Vector3(0.0, 0.0, 1.4), Vector3(), 0, true));

    ASSERT_EQ(1, pixel.getFragmentCount());

    pixel.pushFragment(CanvasFragment(true, Vector3(0.0, 0.0, 1.4), Vector3(), 1, true));

    ASSERT_EQ(1, pixel.getFragmentCount());

    EXPECT_EQ(pixel.getFragment(0).getClient(), 0);
}
