#ifndef RENDER_TEST_H
#define RENDER_TEST_H

#include "BaseTestCase.h"

class Render_Test: public BaseTestCase
{
public:
    CPPUNIT_TEST_SUITE(Render_Test);
    CPPUNIT_TEST(test_render_quad);
    CPPUNIT_TEST_SUITE_END();

    void test_render_quad();
};

#endif // RENDER_TEST_H
