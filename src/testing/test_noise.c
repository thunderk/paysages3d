#include "testing/common.h"

#include "rendering/noise.h"

START_TEST(test_noise_range)
{
    NoiseGenerator* noise;
    double minvalue, maxvalue;

    noise = noiseCreateGenerator();

    noiseAddLevelSimple(noise, 0.1, -1.0, 1.0);
    noiseAddLevelSimple(noise, 0.2, -0.5, 0.2);
    noiseAddLevelSimple(noise, 0.4, -0.3, 1.2);
    noiseValidate(noise);
    noiseGetRange(noise, &minvalue, &maxvalue);

    ck_assert_double_eq(minvalue, -1.8);
    ck_assert_double_eq(maxvalue, 2.4);
}
END_TEST

START_TEST(test_noise_normalize)
{
    int x;
    NoiseGenerator* noise;

    noise = noiseCreateGenerator();

    /* Symmetric case */
    noiseAddLevelsSimple(noise, 10, 1.0, -4.0, 4.0, 0.5);
    noiseValidate(noise);
    noiseNormalizeAmplitude(noise, -1.0, 1.0, 0);
    for (x = 0; x < 1000; x++)
    {
        double value = noiseGet1DTotal(noise, 0.01 * (double)x);
        ck_assert_double_in_range(value, -1.0, 1.0);
    }

    /* Target center differs from current center */
    noiseClearLevels(noise);
    noiseAddLevelsSimple(noise, 10, 1.0, -5.0, 5.0, 0.5);
    noiseValidate(noise);
    noiseNormalizeAmplitude(noise, 0.0, 1.0, 0);
    for (x = 0; x < 1000; x++)
    {
        double value = noiseGet1DTotal(noise, 0.01 * (double)x);
        ck_assert_double_in_range(value, 0.0, 1.0);
    }

    /* Asymmetric range */
    noiseClearLevels(noise);
    noiseAddLevelsSimple(noise, 10, 1.0, 0.0, 10.0, 0.0);
    noiseValidate(noise);
    noiseNormalizeAmplitude(noise, 0.0, 1.0, 0);
    for (x = 0; x < 1000; x++)
    {
        double value = noiseGet1DTotal(noise, 0.01 * (double)x);
        ck_assert_double_in_range(value, 0.0, 1.0);
    }

    /* Complex asymmetric case */
    noiseClearLevels(noise);
    noiseAddLevelsSimple(noise, 3, 1.0, -2.0, 8.0, 0.3);
    noiseValidate(noise);
    noiseNormalizeAmplitude(noise, -2.0, 4.0, 0.0);
    for (x = 0; x < 1000; x++)
    {
        double value = noiseGet1DTotal(noise, 0.01 * (double)x);
        ck_assert_double_in_range(value, -2.0, 4.0);
    }

    noiseDeleteGenerator(noise);
}
END_TEST

TEST_CASE(noise, test_noise_range, test_noise_normalize)

