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
