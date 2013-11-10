#ifndef PACKSTREAM_TEST_H
#define PACKSTREAM_TEST_H

#include "BaseTestCase.h"

class PackStream_Test: public BaseTestCase
{
public:
    CPPUNIT_TEST_SUITE(PackStream_Test);
    CPPUNIT_TEST(testPack);
    CPPUNIT_TEST_SUITE_END();

    void testPack();
};

#endif // PACKSTREAM_TEST_H
