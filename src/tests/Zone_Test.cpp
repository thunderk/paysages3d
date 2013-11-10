#include "Zone_Test.h"
CPPUNIT_TEST_SUITE_REGISTRATION(Zone_Test);

#include "rendering/tools/zone.h"

Zone_Test::testZoneAbsoluteHeight()
{
    Zone* zone = zoneCreate();

    zoneAddHeightRangeQuick(zone, 1.0, -1.0, 2.0, 5.0, 6.0);

    ck_assert_double_eq(zoneGetValue(zone, v3(0.0, -10.0, 0.0), VECTOR_UP), 0.0);
    ck_assert_double_eq(zoneGetValue(zone, v3(0.0, -2.0, 0.0), VECTOR_UP), 0.0);
    ck_assert_double_eq(zoneGetValue(zone, v3(0.0, -1.0, 0.0), VECTOR_UP), 0.0);
    ck_assert_double_eq(zoneGetValue(zone, v3(0.0, -0.5, 0.0), VECTOR_UP), 1.0 / 6.0);
    ck_assert_double_eq(zoneGetValue(zone, v3(0.0, 0.0, 0.0), VECTOR_UP), 1.0 / 3.0);
    ck_assert_double_eq(zoneGetValue(zone, v3(0.0, 0.5, 0.0), VECTOR_UP), 0.5);
    ck_assert_double_eq(zoneGetValue(zone, v3(0.0, 2.0, 0.0), VECTOR_UP), 1.0);
    ck_assert_double_eq(zoneGetValue(zone, v3(0.0, 2.1, 0.0), VECTOR_UP), 1.0);
    ck_assert_double_eq(zoneGetValue(zone, v3(0.0, 3.5, 0.0), VECTOR_UP), 1.0);
    ck_assert_double_eq(zoneGetValue(zone, v3(0.0, 4.9, 0.0), VECTOR_UP), 1.0);
    ck_assert_double_eq(zoneGetValue(zone, v3(0.0, 5.0, 0.0), VECTOR_UP), 1.0);
    ck_assert_double_eq(zoneGetValue(zone, v3(0.0, 5.2, 0.0), VECTOR_UP), 0.8);
    ck_assert_double_eq(zoneGetValue(zone, v3(0.0, 5.7, 0.0), VECTOR_UP), 0.3);
    ck_assert_double_eq(zoneGetValue(zone, v3(0.0, 6.0, 0.0), VECTOR_UP), 0.0);
    ck_assert_double_eq(zoneGetValue(zone, v3(0.0, 15.0, 0.0), VECTOR_UP), 0.0);
    ck_assert_double_eq(zoneGetValue(zone, v3(0.0, 150.0, 0.0), VECTOR_UP), 0.0);

    zoneDelete(zone);
}

Zone_Test::testZoneRelativeHeight()
{
    Zone* zone = zoneCreate();

    zoneAddHeightRangeQuick(zone, 1.0, 0.2, 0.3, 0.6, 0.9);
    zoneSetRelativeHeight(zone, -2.0, 2.0, 8.0);

    ck_assert_double_eq(zoneGetValue(zone, v3(0.0, -10.0, 0.0), VECTOR_UP), 0.0);
    ck_assert_double_eq(zoneGetValue(zone, v3(0.0, -2.1, 0.0), VECTOR_UP), 0.0);
    ck_assert_double_eq(zoneGetValue(zone, v3(0.0, -2.0, 0.0), VECTOR_UP), 0.0);
    ck_assert_double_eq(zoneGetValue(zone, v3(0.0, -1.0, 0.0), VECTOR_UP), 0.0);
    ck_assert_double_eq(zoneGetValue(zone, v3(0.0, -0.5, 0.0), VECTOR_UP), 0.0);
    ck_assert_double_eq(zoneGetValue(zone, v3(0.0, -0.2, 0.0), VECTOR_UP), 0.25);
    ck_assert_double_eq(zoneGetValue(zone, v3(0.0, 0.0, 0.0), VECTOR_UP), 0.5);
    ck_assert_double_eq(zoneGetValue(zone, v3(0.0, 0.3, 0.0), VECTOR_UP), 0.875);
    ck_assert_double_eq(zoneGetValue(zone, v3(0.0, 0.5, 0.0), VECTOR_UP), 1.0);
    ck_assert_double_eq(zoneGetValue(zone, v3(0.0, 1.0, 0.0), VECTOR_UP), 1.0);

    zoneDelete(zone);
}
