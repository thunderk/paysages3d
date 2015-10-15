#include "BaseTestCase.h"

#include "InfiniteRay.h"
#include "InfinitePlane.h"

TEST(InfinitePlane, checkRayIntersection)
{
    InfinitePlane plane(VECTOR_UP, VECTOR_UP);

    int result;
    Vector3 point;

    result = plane.checkRayIntersection(InfiniteRay(Vector3(0.0, 2.0, 0.0), VECTOR_SOUTH), &point);
    ASSERT_EQ(0, result);

    result = plane.checkRayIntersection(InfiniteRay(Vector3(1.5, 2.0, -1.0), VECTOR_DOWN), &point);
    ASSERT_EQ(1, result);
    EXPECT_VECTOR3_COORDS(point, 1.5, 1.0, -1.0);

    result = plane.checkRayIntersection(InfiniteRay(Vector3(1.0, 1.0, 0.0), VECTOR_EAST), &point);
    ASSERT_EQ(-1, result);
}

TEST(InfinitePlane, checkRayIntersection_oblique)
{
    InfinitePlane plane(Vector3(14.0, -5.0, 3.5), Vector3(1.0, 0.0, 0.0));
    InfiniteRay ray(Vector3(2.0, 2.0, 2.0), Vector3(1.0, 1.0, 1.0).normalize());
    int result;
    Vector3 point;

    result = plane.checkRayIntersection(ray, &point);
    ASSERT_EQ(1, result);
    EXPECT_VECTOR3_COORDS(point, 14.0, 14.0, 14.0);
}
