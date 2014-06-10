#include "BaseTestCase.h"

#include "CanvasPortion.h"
#include "CanvasFragment.h"

TEST(CanvasPortion, setSize)
{
    CanvasPortion portion;

    portion.setSize(150, 30);

    EXPECT_EQ(150, portion.getWidth());
    EXPECT_EQ(30, portion.getHeight());
}

TEST(CanvasPortion, pushFragment)
{
    CanvasPortion portion;
    CanvasFragment pushed;
    const CanvasFragment *got;
    int count;

    portion.setSize(50, 50);
    portion.pushFragment(10, 15, pushed);

    count = portion.getFragmentCount(10, 14);
    ASSERT_EQ(0, count);
    got = portion.getFrontFragment(10, 14);
    ASSERT_FALSE(got);

    count = portion.getFragmentCount(10, 15);
    ASSERT_EQ(1, count);
    got = portion.getFrontFragment(10, 15);
    ASSERT_TRUE(got);
}

TEST(CanvasPortion, pushFragment_opaque)
{
    CanvasPortion portion;
    CanvasFragment pushed;

    portion.setSize(10, 10);

    pushed = CanvasFragment(2.0, VECTOR_ZERO, 0);
    portion.pushFragment(2, 2, pushed);

    ASSERT_EQ(1, portion.getFragmentCount(2, 2));
    EXPECT_DOUBLE_EQ(2.0, portion.getFrontFragment(2, 2)->getZ());

    pushed = CanvasFragment(4.0, VECTOR_ZERO, 0);
    portion.pushFragment(2, 2, pushed);

    ASSERT_EQ(1, portion.getFragmentCount(2, 2));
    EXPECT_DOUBLE_EQ(2.0, portion.getFrontFragment(2, 2)->getZ());

    pushed = CanvasFragment(1.0, VECTOR_ZERO, 0);
    portion.pushFragment(2, 2, pushed);

    ASSERT_EQ(1, portion.getFragmentCount(2, 2));
    EXPECT_DOUBLE_EQ(1.0, portion.getFrontFragment(2, 2)->getZ());
}

TEST(CanvasPortion, pushFragment_transparent)
{
    CanvasPortion portion;
    CanvasFragment pushed;

    portion.setSize(10, 10);

    pushed = CanvasFragment(4.0, VECTOR_ZERO, 0, false);
    portion.pushFragment(2, 2, pushed);

    ASSERT_EQ(1, portion.getFragmentCount(2, 2));
    EXPECT_DOUBLE_EQ(4.0, portion.getFrontFragment(2, 2)->getZ());

    pushed = CanvasFragment(3.0, VECTOR_ZERO, 0, true);
    portion.pushFragment(2, 2, pushed);

    ASSERT_EQ(1, portion.getFragmentCount(2, 2));
    EXPECT_DOUBLE_EQ(3.0, portion.getFrontFragment(2, 2)->getZ());

    pushed = CanvasFragment(2.0, VECTOR_ZERO, 0, false);
    portion.pushFragment(2, 2, pushed);

    ASSERT_EQ(2, portion.getFragmentCount(2, 2));
    EXPECT_DOUBLE_EQ(2.0, portion.getFrontFragment(2, 2)->getZ());
}

TEST(CanvasPortion, clear)
{
    CanvasPortion portion;
    CanvasFragment fragment;

    portion.setSize(10, 10);
    portion.pushFragment(5, 5, fragment);

    EXPECT_EQ(0, portion.getFragmentCount(4, 5));
    EXPECT_EQ(1, portion.getFragmentCount(5, 5));

    portion.clear();

    EXPECT_EQ(0, portion.getFragmentCount(4, 5));
    EXPECT_EQ(0, portion.getFragmentCount(5, 5));
}
