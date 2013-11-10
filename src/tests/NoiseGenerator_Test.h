#ifndef NOISEGENERATOR_TEST_H
#define NOISEGENERATOR_TEST_H

#include "BaseTestCase.h"

class NoiseGenerator_Test: public BaseTestCase
{
public:
    CPPUNIT_TEST_SUITE(NoiseGenerator_Test);
    CPPUNIT_TEST(test_noise_range);
    CPPUNIT_TEST(test_noise_normalize);
    CPPUNIT_TEST_SUITE_END();

    void test_noise_range();
    void test_noise_normalize();
};

#endif // NOISEGENERATOR_TEST_H
