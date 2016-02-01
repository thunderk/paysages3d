#include "BaseTestCase.h"
#include "Maths.h"

TEST(Maths, modInRange) {
    EXPECT_DOUBLE_EQ(0.7, Maths::modInRange(-0.3, 0.0, 1.0));
    EXPECT_DOUBLE_EQ(0.0, Maths::modInRange(0.0, 0.0, 1.0));
    EXPECT_DOUBLE_EQ(0.4, Maths::modInRange(0.4, 0.0, 1.0));
    EXPECT_DOUBLE_EQ(0.0, Maths::modInRange(1.0, 0.0, 1.0));
    EXPECT_DOUBLE_EQ(0.5, Maths::modInRange(1.5, 0.0, 1.0));

    EXPECT_DOUBLE_EQ(5.2831853071795862, Maths::modInRange(-1.0, 0, Maths::TWOPI));
    EXPECT_DOUBLE_EQ(-1.183185307179586, Maths::modInRange(5.1, -Maths::PI, Maths::PI));
}

TEST(Maths, zeroPoint) {
    EXPECT_DOUBLE_EQ(0.5, Maths::zeroPoint(1.0, 1.0, -1.0));
    EXPECT_DOUBLE_EQ(4.0, Maths::zeroPoint(2.0, 1.0, 0.5));
    EXPECT_DOUBLE_EQ(-1.5, Maths::zeroPoint(1.5, -1.0, -2.0));
    EXPECT_DOUBLE_EQ(0.0, Maths::zeroPoint(0.0, 1.0, 1.0));
}

TEST(Maths, smoothstep) {
    EXPECT_DOUBLE_EQ(0.0, Maths::smoothstep(2.0, 5.0, -1.0));
    EXPECT_DOUBLE_EQ(0.0, Maths::smoothstep(2.0, 5.0, 1.0));
    EXPECT_DOUBLE_EQ(0.0, Maths::smoothstep(2.0, 5.0, 2.0));
    EXPECT_DOUBLE_EQ(0.07407407407407407, Maths::smoothstep(2.0, 5.0, 2.5));
    EXPECT_DOUBLE_EQ(0.25925925925925924, Maths::smoothstep(2.0, 5.0, 3.0));
    EXPECT_DOUBLE_EQ(0.5, Maths::smoothstep(2.0, 5.0, 3.5));
    EXPECT_DOUBLE_EQ(1.0, Maths::smoothstep(2.0, 5.0, 5.0));
    EXPECT_DOUBLE_EQ(1.0, Maths::smoothstep(2.0, 5.0, 10.0));
}
