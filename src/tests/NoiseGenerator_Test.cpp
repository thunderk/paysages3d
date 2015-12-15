#include "BaseTestCase.h"

#include "NoiseGenerator.h"

TEST(NoiseGenerator, getRange) {
    NoiseGenerator noise;
    double minvalue, maxvalue;

    noise.addLevelSimple(0.1, -1.0, 1.0);
    noise.addLevelSimple(0.2, -0.5, 0.2);
    noise.addLevelSimple(0.4, -0.3, 1.2);
    noise.validate();
    noise.getRange(&minvalue, &maxvalue);

    EXPECT_DOUBLE_EQ(minvalue, -1.8);
    EXPECT_DOUBLE_EQ(maxvalue, 2.4);
}

TEST(NoiseGenerator, normalizeAmplitude) {
    int x;
    NoiseGenerator noise;

    /* Symmetric case */
    noise.addLevelsSimple(10, 1.0, -4.0, 4.0, 0.5);
    noise.validate();
    noise.normalizeAmplitude(-1.0, 1.0, 0);
    for (x = 0; x < 1000; x++) {
        double value = noise.get1DTotal(0.01 * to_double(x));
        ASSERT_DOUBLE_IN_RANGE(value, -1.0, 1.0);
    }

    /* Target center differs from current center */
    noise.clearLevels();
    noise.addLevelsSimple(10, 1.0, -5.0, 5.0, 0.5);
    noise.validate();
    noise.normalizeAmplitude(0.0, 1.0, 0);
    for (x = 0; x < 1000; x++) {
        double value = noise.get1DTotal(0.01 * to_double(x));
        ASSERT_DOUBLE_IN_RANGE(value, 0.0, 1.0);
    }

    /* Asymmetric range */
    noise.clearLevels();
    noise.addLevelsSimple(10, 1.0, 0.0, 10.0, 0.0);
    noise.validate();
    noise.normalizeAmplitude(0.0, 1.0, 0);
    for (x = 0; x < 1000; x++) {
        double value = noise.get1DTotal(0.01 * to_double(x));
        ASSERT_DOUBLE_IN_RANGE(value, 0.0, 1.0);
    }

    /* Complex asymmetric case */
    noise.clearLevels();
    noise.addLevelsSimple(3, 1.0, -2.0, 8.0, 0.3);
    noise.validate();
    noise.normalizeAmplitude(-2.0, 4.0, 0.0);
    for (x = 0; x < 1000; x++) {
        double value = noise.get1DTotal(0.01 * to_double(x));
        ASSERT_DOUBLE_IN_RANGE(value, -2.0, 4.0);
    }
}

TEST(NoiseGenerator, forceValue) {
    NoiseGenerator noise;
    noise.addLevelsSimple(10, 1.0, -4.0, 4.0, 0.5);

    noise.forceValue(2.3);
    noise.validate();

    EXPECT_DOUBLE_EQ(2.3, noise.get1DTotal(0.5));
    EXPECT_DOUBLE_EQ(2.3, noise.get2DTotal(-6.3, 1.4));
    EXPECT_DOUBLE_EQ(2.3, noise.get3DTotal(12.9, 0.0, -5.0));
}
