#include "BaseTestCase.h"
#include "Interpolation.h"

TEST(Interpolation, trilinear) {
    double p[8] = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0};

    EXPECT_DOUBLE_EQ(0.0, Interpolation::trilinear(p, 0.0, 0.0, 0.0));
    EXPECT_DOUBLE_EQ(1.0, Interpolation::trilinear(p, 1.0, 0.0, 0.0));
    EXPECT_DOUBLE_EQ(2.0, Interpolation::trilinear(p, 1.0, 1.0, 0.0));
    EXPECT_DOUBLE_EQ(3.0, Interpolation::trilinear(p, 0.0, 1.0, 0.0));
    EXPECT_DOUBLE_EQ(4.0, Interpolation::trilinear(p, 0.0, 0.0, 1.0));
    EXPECT_DOUBLE_EQ(5.0, Interpolation::trilinear(p, 1.0, 0.0, 1.0));
    EXPECT_DOUBLE_EQ(6.0, Interpolation::trilinear(p, 1.0, 1.0, 1.0));
    EXPECT_DOUBLE_EQ(7.0, Interpolation::trilinear(p, 0.0, 1.0, 1.0));

    EXPECT_DOUBLE_EQ(0.5, Interpolation::trilinear(p, 0.5, 0.0, 0.0));
    EXPECT_DOUBLE_EQ(1.5, Interpolation::trilinear(p, 0.0, 0.5, 0.0));
    EXPECT_DOUBLE_EQ(2.0, Interpolation::trilinear(p, 0.0, 0.0, 0.5));

    EXPECT_DOUBLE_EQ(1.5, Interpolation::trilinear(p, 0.5, 0.5, 0.0));
    EXPECT_DOUBLE_EQ(3.5, Interpolation::trilinear(p, 0.0, 0.5, 0.5));
    EXPECT_DOUBLE_EQ(2.5, Interpolation::trilinear(p, 0.5, 0.0, 0.5));

    EXPECT_DOUBLE_EQ(3.5, Interpolation::trilinear(p, 0.5, 0.5, 0.5));
}
