#ifndef CLOUDS_TEST_H
#define CLOUDS_TEST_H

#include "BaseTestCase.h"

class Clouds_Test: public BaseTestCase
{
public:
    CPPUNIT_TEST_SUITE(Clouds_Test);
    CPPUNIT_TEST(test_clouds_density);
    CPPUNIT_TEST(test_clouds_walking_boundaries);
    CPPUNIT_TEST(test_clouds_walking);
    CPPUNIT_TEST(test_clouds_walking_local);
    CPPUNIT_TEST_SUITE_END();

    void test_clouds_density();
    void test_clouds_walking_boundaries();
    void test_clouds_walking();
    void test_clouds_walking_local();
};

#endif // CLOUDS_TEST_H
