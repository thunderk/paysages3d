#include "BaseTestCase.h"

#include "FractalNoise.h"

class TestFractalNoise : public FractalNoise {
  public:
    TestFractalNoise(double value = 0.0) : FractalNoise(), value(value) {
        NoiseState state;
        state.resetOffsets();
        setState(state);

        calls = 0;
    }

    virtual double getBase3d(double x, double y, double z) const override {
        ((TestFractalNoise *)this)->calls++;
        return value + x + y + z;
    }

  public:
    int calls;

  private:
    double value;
};

TEST(FractalNoise, AutoBase2d) {
    TestFractalNoise noise(0.2);

    double result = noise.getBase2d(0.4, 0.1);

    ASSERT_EQ(1, noise.calls);
    EXPECT_DOUBLE_EQ(0.7, result);
}

TEST(FractalNoise, AutoBase1d) {
    TestFractalNoise noise(0.2);

    double result = noise.getBase1d(0.4);

    ASSERT_EQ(1, noise.calls);
    EXPECT_DOUBLE_EQ(0.6, result);
}

TEST(FractalNoise, NoStep) {
    TestFractalNoise noise(0.1);

    double result = noise.get1d(1.1, 1.0);

    ASSERT_EQ(0, noise.calls);
    EXPECT_DOUBLE_EQ(0.0, result);
}

TEST(FractalNoise, SingleStep) {
    TestFractalNoise noise(0.1);

    double result = noise.get1d(0.7, 1.0);

    ASSERT_EQ(1, noise.calls);
    EXPECT_DOUBLE_EQ(1.1, result);
}

TEST(FractalNoise, Sum2Steps) {
    TestFractalNoise noise(0.1);

    double result = noise.get1d(0.3, 1.0);

    ASSERT_EQ(2, noise.calls);
    EXPECT_DOUBLE_EQ(1.1 + 1.05, result);
}

TEST(FractalNoise, Sum3Steps) {
    TestFractalNoise noise(0.1);

    double result = noise.get1d(0.2, 1.0);

    ASSERT_EQ(3, noise.calls);
    EXPECT_DOUBLE_EQ(1.1 + 1.05 + 1.025, result);
}

TEST(FractalNoise, InitialScaling) {
    TestFractalNoise noise(0.8);
    noise.setScaling(0.5);

    double result = noise.get1d(0.2, 1.0);

    ASSERT_EQ(2, noise.calls);
    EXPECT_DOUBLE_EQ(1.4 + 1.2, result);
}

TEST(FractalNoise, InitialHeight) {
    TestFractalNoise noise(0.8);
    noise.setScaling(1.0, 0.5);

    double result = noise.get1d(0.2, 1.0);

    ASSERT_EQ(2, noise.calls);
    EXPECT_DOUBLE_EQ(0.9 + 0.7, result);
}

TEST(FractalNoise, InitialScalingAndHeight) {
    TestFractalNoise noise(0.8);
    noise.setScaling(0.5, 0.5);

    double result = noise.get1d(0.1, 1.0);

    ASSERT_EQ(2, noise.calls);
    EXPECT_DOUBLE_EQ(0.7 + 0.6, result);
}

TEST(FractalNoise, StepScaling) {
    TestFractalNoise noise(0.1);
    noise.setStep(0.8);

    double result = noise.get1d(0.55, 1.0);

    ASSERT_EQ(3, noise.calls);
    EXPECT_DOUBLE_EQ(1.1 + 1.08 + 1.064, result);
}

TEST(FractalNoise, StepHeight) {
    TestFractalNoise noise(0.1);
    noise.setStep(0.5, 0.8);

    double result = noise.get1d(0.4, 1.0);

    ASSERT_EQ(2, noise.calls);
    EXPECT_DOUBLE_EQ(1.1 + 0.84, result);
}

TEST(FractalNoise, StepScalingAndHeight) {
    TestFractalNoise noise(0.8);
    noise.setStep(0.5, 0.5);

    double result = noise.get1d(0.05, 1.0);

    ASSERT_EQ(3, noise.calls);
    EXPECT_DOUBLE_EQ(1.8 + 0.7 + 0.3, result);
}

TEST(FractalNoise, StateOffset) {
    TestFractalNoise noise(0.8);
    NoiseState state;
    state.resetOffsets(0.2);
    noise.setState(state);

    double result = noise.get1d(0.4, 1.0);

    ASSERT_EQ(2, noise.calls);
    EXPECT_DOUBLE_EQ(2.0 + 1.5, result);
}

TEST(FractalNoise, StateOffsetIter) {
    TestFractalNoise noise(0.8);
    NoiseState state;
    state.setLevel(0, 0.1, 0.1, 0.1);
    state.setLevel(1, 0.6, 0.6, 0.6);
    noise.setState(state);

    double result = noise.get1d(0.4, 1.0);

    ASSERT_EQ(2, noise.calls);
    EXPECT_DOUBLE_EQ(1.9 + 1.7, result);
}

TEST(FractalNoise, StateOffsetLoop) {
    TestFractalNoise noise(0.8);
    NoiseState state;
    state.setLevelCount(2);
    state.setLevel(0, 0.1, 0.1, 0.1);
    state.setLevel(1, 0.6, 0.6, 0.6);
    noise.setState(state);

    double result = noise.get1d(0.2, 1.0);

    ASSERT_EQ(3, noise.calls);
    EXPECT_DOUBLE_EQ(1.9 + 1.7 + 1.225, result);
}

TEST(FractalNoise, Noise2d) {
    TestFractalNoise noise(0.8);
    NoiseState state;
    state.setLevelCount(2);
    state.setLevel(0, 0.1, 0.2, 0.3);
    state.setLevel(1, 0.6, 0.7, 0.8);
    noise.setState(state);

    double result = noise.get2d(0.4, 1.0, 0.5);

    ASSERT_EQ(2, noise.calls);
    EXPECT_DOUBLE_EQ(2.6 + 2.55, result);
}

TEST(FractalNoise, Noise3d) {
    TestFractalNoise noise(0.8);
    NoiseState state;
    state.setLevelCount(2);
    state.setLevel(0, 0.1, 0.2, 0.3);
    state.setLevel(1, 0.6, 0.7, 0.8);
    noise.setState(state);

    double result = noise.get3d(0.4, 1.0, 0.5, 0.3);

    ASSERT_EQ(2, noise.calls);
    EXPECT_DOUBLE_EQ(3.2 + 3.25, result);
}
