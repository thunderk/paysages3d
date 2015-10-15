#include "BaseTestCase.h"

#include "CappedCylinder.h"

TEST(CappedCylinder, checkRayIntersection)
{
    CappedCylinder cylinder(VECTOR_DOWN, VECTOR_UP, 1.0, 2.0);

    int intersect_count;
    Vector3 p1, p2;

    intersect_count = cylinder.checkRayIntersection(InfiniteRay(Vector3(1.5, 0.0, 0.0), Vector3(0.0, 0.0, 1.0)), &p1, &p2);
    EXPECT_EQ(0, intersect_count);

    intersect_count = cylinder.checkRayIntersection(InfiniteRay(Vector3(1.0, 0.0, 0.0), Vector3(0.0, 0.0, 1.0)), &p1, &p2);
    EXPECT_EQ(1, intersect_count);
    EXPECT_VECTOR3_COORDS(p1, 1.0, 0.0, 0.0);

    intersect_count = cylinder.checkRayIntersection(InfiniteRay(Vector3(0.5, 0.0, 0.0), Vector3(0.0, 0.0, 1.0)), &p1, &p2);
    EXPECT_EQ(2, intersect_count);
    EXPECT_VECTOR3_COORDS(p1, 0.5, 0.0, -cos(asin(0.5)));
    EXPECT_VECTOR3_COORDS(p2, 0.5, 0.0, cos(asin(0.5)));

    intersect_count = cylinder.checkRayIntersection(InfiniteRay(Vector3(0.5, -2.1, 0.0), Vector3(0.0, 0.0, 1.0)), &p1, &p2);
    EXPECT_EQ(0, intersect_count);

    intersect_count = cylinder.checkRayIntersection(InfiniteRay(Vector3(0.5, 2.1, 0.0), Vector3(0.0, 0.0, 1.0)), &p1, &p2);
    EXPECT_EQ(0, intersect_count);

    // diagonal cases (through a cap)
    intersect_count = cylinder.checkRayIntersection(InfiniteRay(Vector3(-2.0, -1.0, 0.0), Vector3(1.0, 1.0, 0.0)), &p1, &p2);
    EXPECT_EQ(1, intersect_count);
    EXPECT_VECTOR3_COORDS(p1, -1.0, 0.0, 0.0);

    intersect_count = cylinder.checkRayIntersection(InfiniteRay(Vector3(-2.0, 3.0, 0.0), Vector3(1.0, -1.0, 0.0)), &p1, &p2);
    EXPECT_EQ(1, intersect_count);
    EXPECT_VECTOR3_COORDS(p1, 1.0, 0.0, 0.0);
}
