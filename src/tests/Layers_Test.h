#ifndef LAYERS_TEST_H
#define LAYERS_TEST_H

#include "BaseTestCase.h"

class Layers_Test: public BaseTestCase
{
public:
    CPPUNIT_TEST_SUITE(Layers_Test);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testNullLayer);
    CPPUNIT_TEST(testCopy);
    CPPUNIT_TEST(testMaxLayerCount);
    CPPUNIT_TEST_SUITE_END();

    void testConstructor();
    void testNullLayer();
    void testCopy();
    void testMaxLayerCount();
};

#endif // LAYERS_TEST_H
