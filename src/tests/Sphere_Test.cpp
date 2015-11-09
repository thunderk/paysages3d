#include "BaseTestCase.h"

#include "InfiniteRay.h"
#include "Sphere.h"

TEST(Sphere, checkRayIntersection) {
    Sphere sphere(Vector3(2.0, 1.0, 1.0), 0.5);

    int intersect_count;
    Vector3 p1, p2;

    intersect_count = sphere.checkRayIntersection(InfiniteRay(Vector3(0.0, 0.0, 0.0), VECTOR_SOUTH), &p1, &p2);
    ASSERT_EQ(0, intersect_count);

    intersect_count = sphere.checkRayIntersection(InfiniteRay(Vector3(1.5, 1.0, 0.0), VECTOR_SOUTH), &p1, &p2);
    ASSERT_EQ(1, intersect_count);
    EXPECT_VECTOR3_COORDS(p1, 1.5, 1.0, 1.0);

    intersect_count = sphere.checkRayIntersection(InfiniteRay(Vector3(2.0, 1.0, 0.0), VECTOR_SOUTH), &p1, &p2);
    ASSERT_EQ(2, intersect_count);
    EXPECT_VECTOR3_COORDS(p1, 2.0, 1.0, 0.5);
    EXPECT_VECTOR3_COORDS(p2, 2.0, 1.0, 1.5);
}
