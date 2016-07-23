#include "BaseTestCase.h"

#include "InfiniteCylinder.h"
#include "InfiniteRay.h"
#include <cmath>

TEST(InfiniteCylinder, findRayIntersection) {
    InfiniteRay ray(VECTOR_ZERO, VECTOR_UP);
    InfiniteCylinder cylinder(ray, 1.0);

    int intersect_count;
    Vector3 p1, p2;

    intersect_count =
        cylinder.findRayIntersection(InfiniteRay(Vector3(1.5, 0.0, 0.0), Vector3(0.0, 0.0, 1.0)), &p1, &p2);
    EXPECT_EQ(0, intersect_count);

    intersect_count =
        cylinder.findRayIntersection(InfiniteRay(Vector3(1.0, 0.0, 0.0), Vector3(0.0, 0.0, 1.0)), &p1, &p2);
    EXPECT_EQ(1, intersect_count);
    EXPECT_VECTOR3_COORDS(p1, 1.0, 0.0, 0.0);

    intersect_count =
        cylinder.findRayIntersection(InfiniteRay(Vector3(0.5, 0.0, 0.0), Vector3(0.0, 0.0, 1.0)), &p1, &p2);
    EXPECT_EQ(2, intersect_count);
    EXPECT_VECTOR3_COORDS(p1, 0.5, 0.0, -cos(asin(0.5)));
    EXPECT_VECTOR3_COORDS(p2, 0.5, 0.0, cos(asin(0.5)));
}

TEST(InfiniteCylinder, getRayIntersection2) {
    InfiniteRay ray(Vector3(-1.4, 1.5, 1.0), Vector3(1.0, 0.0, 0.0));
    InfiniteCylinder cylinder(ray, 0.5);

    int intersect_count;
    Vector3 p1, p2;

    intersect_count =
        cylinder.findRayIntersection(InfiniteRay::fromPoints(Vector3(0.0, 1.5, 0.0), Vector3(0.0, 1.5, 2.0)), &p1, &p2);
    EXPECT_EQ(2, intersect_count);
    EXPECT_VECTOR3_COORDS(p1, 0.0, 1.5, 0.5);
    EXPECT_VECTOR3_COORDS(p2, 0.0, 1.5, 1.5);
}
