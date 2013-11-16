#include "BaseTestCase.h"

#include "Zone.h"
#include "Vector3.h"

class Zone_Test : public BaseTestCase {
protected:
    virtual void SetUp() {
        zone = new Zone;
    }

    virtual void TearDown()
    {
        delete zone;
    }

    Zone* zone;
};

TEST_F(Zone_Test, absolute_height)
{
    zone->addHeightRangeQuick(1.0, -1.0, 2.0, 5.0, 6.0);

    EXPECT_DOUBLE_EQ(zone->getValue(Vector3(0.0, -10.0, 0.0), VECTOR_UP), 0.0);
    EXPECT_DOUBLE_EQ(zone->getValue(Vector3(0.0, -2.0, 0.0), VECTOR_UP), 0.0);
    EXPECT_DOUBLE_EQ(zone->getValue(Vector3(0.0, -1.0, 0.0), VECTOR_UP), 0.0);
    EXPECT_DOUBLE_EQ(zone->getValue(Vector3(0.0, -0.5, 0.0), VECTOR_UP), 1.0 / 6.0);
    EXPECT_DOUBLE_EQ(zone->getValue(Vector3(0.0, 0.0, 0.0), VECTOR_UP), 1.0 / 3.0);
    EXPECT_DOUBLE_EQ(zone->getValue(Vector3(0.0, 0.5, 0.0), VECTOR_UP), 0.5);
    EXPECT_DOUBLE_EQ(zone->getValue(Vector3(0.0, 2.0, 0.0), VECTOR_UP), 1.0);
    EXPECT_DOUBLE_EQ(zone->getValue(Vector3(0.0, 2.1, 0.0), VECTOR_UP), 1.0);
    EXPECT_DOUBLE_EQ(zone->getValue(Vector3(0.0, 3.5, 0.0), VECTOR_UP), 1.0);
    EXPECT_DOUBLE_EQ(zone->getValue(Vector3(0.0, 4.9, 0.0), VECTOR_UP), 1.0);
    EXPECT_DOUBLE_EQ(zone->getValue(Vector3(0.0, 5.0, 0.0), VECTOR_UP), 1.0);
    EXPECT_DOUBLE_EQ(zone->getValue(Vector3(0.0, 5.2, 0.0), VECTOR_UP), 0.8);
    EXPECT_DOUBLE_EQ(zone->getValue(Vector3(0.0, 5.7, 0.0), VECTOR_UP), 0.3);
    EXPECT_DOUBLE_EQ(zone->getValue(Vector3(0.0, 6.0, 0.0), VECTOR_UP), 0.0);
    EXPECT_DOUBLE_EQ(zone->getValue(Vector3(0.0, 15.0, 0.0), VECTOR_UP), 0.0);
    EXPECT_DOUBLE_EQ(zone->getValue(Vector3(0.0, 150.0, 0.0), VECTOR_UP), 0.0);
}

TEST_F(Zone_Test, relative_height)
{
    zone->addHeightRangeQuick(1.0, 0.2, 0.3, 0.6, 0.9);
    zone->setRelativeHeight(-2.0, 2.0, 8.0);

    EXPECT_DOUBLE_EQ(zone->getValue(Vector3(0.0, -10.0, 0.0), VECTOR_UP), 0.0);
    EXPECT_DOUBLE_EQ(zone->getValue(Vector3(0.0, -2.1, 0.0), VECTOR_UP), 0.0);
    EXPECT_DOUBLE_EQ(zone->getValue(Vector3(0.0, -2.0, 0.0), VECTOR_UP), 0.0);
    EXPECT_DOUBLE_EQ(zone->getValue(Vector3(0.0, -1.0, 0.0), VECTOR_UP), 0.0);
    EXPECT_DOUBLE_EQ(zone->getValue(Vector3(0.0, -0.5, 0.0), VECTOR_UP), 0.0);
    EXPECT_DOUBLE_EQ(zone->getValue(Vector3(0.0, -0.2, 0.0), VECTOR_UP), 0.25);
    EXPECT_DOUBLE_EQ(zone->getValue(Vector3(0.0, 0.0, 0.0), VECTOR_UP), 0.5);
    EXPECT_DOUBLE_EQ(zone->getValue(Vector3(0.0, 0.3, 0.0), VECTOR_UP), 0.875);
    EXPECT_DOUBLE_EQ(zone->getValue(Vector3(0.0, 0.5, 0.0), VECTOR_UP), 1.0);
    EXPECT_DOUBLE_EQ(zone->getValue(Vector3(0.0, 1.0, 0.0), VECTOR_UP), 1.0);
}
