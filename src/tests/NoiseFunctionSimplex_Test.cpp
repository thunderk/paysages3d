#include "NoiseFunctionSimplex.h"
#include "BaseTestCase.h"

#include "RandomGenerator.h"

TEST(NoiseFunctionSimplex, range) {
    NoiseFunctionSimplex noise;
    RandomGenerator random(17124);

    for (int i = 0; i < 100000; i++) {
        double x = random.genDouble() - 0.5;
        double y = random.genDouble() - 0.5;
        double z = random.genDouble() - 0.5;
        double scale = random.genDouble() * 5.0 + 0.1;

        // TODO Test repartition and filling

        double val2 = noise.get2d(1.0, x * scale, y * scale);
        EXPECT_DOUBLE_IN_RANGE(val2, -0.5, 0.5);

        double val3 = noise.get3d(1.0, x * scale, y * scale, z * scale);
        EXPECT_DOUBLE_IN_RANGE(val3, -0.5, 0.5);
    }
}
