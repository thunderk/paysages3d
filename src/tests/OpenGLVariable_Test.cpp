#include "BaseTestCase.h"
#include "OpenGLVariable.h"

#include "NoiseFunctionSimplex.h"

TEST(OpenGLVariable, setNoise) {
    OpenGLVariable var("test");
    NoiseFunctionSimplex noise;

    noise.setScaling(0.5, 2.0);
    noise.setStep(3.0, 0.4);

    var.set(noise);

    EXPECT_EQ(4, var.getIntValue());
    EXPECT_FLOAT_EQ(2.0f, var.getFloatArrayValue(0));
    EXPECT_FLOAT_EQ(1.0f, var.getFloatArrayValue(1));
    EXPECT_FLOAT_EQ(1.0f / 3.0f, var.getFloatArrayValue(2));
    EXPECT_FLOAT_EQ(1.2f, var.getFloatArrayValue(3));
}
