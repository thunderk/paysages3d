#ifndef CAMERA_TEST_H
#define CAMERA_TEST_H

#include "BaseTestCase.h"

class Camera_Test: public BaseTestCase
{
public:
    CPPUNIT_TEST_SUITE(Camera_Test);
    CPPUNIT_TEST(test_camera_definition);
    CPPUNIT_TEST(test_camera_projection);
    CPPUNIT_TEST(test_camera_depth);
    CPPUNIT_TEST_SUITE_END();

    void test_camera_definition();
    void test_camera_projection();
    void test_camera_depth();
};

#endif // CAMERA_TEST_H
