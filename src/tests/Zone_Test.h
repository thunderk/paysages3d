#ifndef ZONE_TEST_H
#define ZONE_TEST_H

#include "BaseTestCase.h"

class Zone_Test: public BaseTestCase
{
public:
    CPPUNIT_TEST_SUITE(Zone_Test);
    CPPUNIT_TEST(testZoneAbsoluteHeight);
    CPPUNIT_TEST(testZoneRelativeHeight);
    CPPUNIT_TEST_SUITE_END();

    void testZoneAbsoluteHeight();
    void testZoneRelativeHeight();
};

#endif // ZONE_TEST_H
