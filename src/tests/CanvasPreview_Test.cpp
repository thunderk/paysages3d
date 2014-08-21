#include "BaseTestCase.h"

#include "CanvasPreview.h"
#include "Color.h"

TEST(CanvasPreview, setSize)
{
    CanvasPreview preview;

    preview.setSize(800, 600, 400, 300);

    EXPECT_EQ(400, preview.getWidth());
    EXPECT_EQ(300, preview.getHeight());
}

TEST(CanvasPreview, pushPixel_accumulate)
{
    CanvasPreview preview;
    Color col;
    preview.setSize(800, 600, 400, 300);

    col = preview.getFinalPixel(0, 0);
    EXPECT_COLOR_RGBA(col, 0.0, 0.0, 0.0, 1.0);

    preview.pushPixel(0, 0, COLOR_BLACK, COLOR_RED);
    col = preview.getFinalPixel(0, 0);
    EXPECT_COLOR_RGBA(col, 0.25, 0.0, 0.0, 1.0);

    preview.pushPixel(0, 1, COLOR_BLACK, COLOR_BLUE);
    col = preview.getFinalPixel(0, 0);
    EXPECT_COLOR_RGBA(col, 0.25, 0.0, 0.25, 1.0);

    preview.pushPixel(0, 2, COLOR_BLACK, COLOR_BLUE);
    col = preview.getFinalPixel(0, 0);
    EXPECT_COLOR_RGBA(col, 0.25, 0.0, 0.25, 1.0);
    col = preview.getFinalPixel(0, 1);
    EXPECT_COLOR_RGBA(col, 0.0, 0.0, 0.25, 1.0);

    preview.pushPixel(0, 1, COLOR_BLUE, COLOR_GREEN);
    col = preview.getFinalPixel(0, 0);
    EXPECT_COLOR_RGBA(col, 0.25, 0.25, 0.0, 1.0);
}

TEST(CanvasPreview, pushPixel_border)
{
    CanvasPreview preview;
    preview.setSize(759, 237, 9, 14);

    preview.pushPixel(759, 237, COLOR_BLACK, COLOR_RED);
}
