#include "BaseTestCase.h"

#include "Canvas.h"
#include "CanvasPortion.h"
#include "CanvasPreview.h"

static void checkPortion(Canvas &canvas, int x, int y, int width, int height)
{
    ASSERT_LT(x, canvas.getHorizontalPortionCount());
    ASSERT_LT(y, canvas.getVerticalPortionCount());

    CanvasPortion* portion = canvas.at(x, y);

    EXPECT_EQ(width, portion->getWidth());
    EXPECT_EQ(height, portion->getHeight());
}

TEST(Canvas, SizingAndCutting)
{
    Canvas canvas;

    canvas.setSize(200, 100);
    EXPECT_EQ(200, canvas.getWidth());
    EXPECT_EQ(100, canvas.getHeight());
    EXPECT_EQ(200, canvas.getPreview()->getWidth());
    EXPECT_EQ(100, canvas.getPreview()->getHeight());
    ASSERT_EQ(1, canvas.getHorizontalPortionCount());
    ASSERT_EQ(1, canvas.getVerticalPortionCount());
    checkPortion(canvas, 0, 0, 200, 100);

    canvas.setSize(600, 501);
    EXPECT_EQ(600, canvas.getWidth());
    EXPECT_EQ(501, canvas.getHeight());
    EXPECT_EQ(600, canvas.getPreview()->getWidth());
    EXPECT_EQ(501, canvas.getPreview()->getHeight());
    ASSERT_EQ(2, canvas.getHorizontalPortionCount());
    ASSERT_EQ(2, canvas.getVerticalPortionCount());
    checkPortion(canvas, 0, 0, 300, 250);
    checkPortion(canvas, 0, 1, 300, 251);
    checkPortion(canvas, 1, 0, 300, 250);
    checkPortion(canvas, 1, 1, 300, 251);
}
