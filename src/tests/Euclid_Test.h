#ifndef EUCLID_TEST_H
#define EUCLID_TEST_H

#include "BaseTestCase.h"

class Euclid_Test: public BaseTestCase
{
public:
    CPPUNIT_TEST_SUITE(Euclid_Test);
    CPPUNIT_TEST(test_camera_definition);
    CPPUNIT_TEST(test_camera_projection);
    CPPUNIT_TEST(test_camera_depth);
    CPPUNIT_TEST_SUITE_END();

    void test_camera_definition();
    void test_camera_projection();
    void test_camera_depth();

};

#endif // EUCLID_TEST_H
