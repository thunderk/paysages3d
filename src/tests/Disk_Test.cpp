#include "BaseTestCase.h"

#include "InfiniteRay.h"
#include "Disk.h"

TEST(Disk, checkRayIntersection)
{
    Disk disk(VECTOR_UP, VECTOR_UP, 0.8);

    int result;
    Vector3 point;

    result = disk.checkRayIntersection(InfiniteRay(Vector3(0.0, 2.0, 0.0), VECTOR_SOUTH), &point);
    ASSERT_EQ(0, result);

    result = disk.checkRayIntersection(InfiniteRay(Vector3(1.5, 2.0, -1.0), VECTOR_DOWN), &point);
    ASSERT_EQ(0, result);

    result = disk.checkRayIntersection(InfiniteRay(Vector3(0.5, 2.0, 0.0), VECTOR_DOWN), &point);
    ASSERT_EQ(1, result);
    EXPECT_VECTOR3_COORDS(point, 0.5, 1.0, 0.0);

    result = disk.checkRayIntersection(InfiniteRay(Vector3(1.0, 1.0, 0.0), VECTOR_EAST), &point);
    ASSERT_EQ(-1, result);
}
