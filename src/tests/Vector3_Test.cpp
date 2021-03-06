#include "Vector3.h"
#include "BaseTestCase.h"

TEST(Vector3, midPointTo) {
    Vector3 v1(1.0, 2.0, 8.0);
    Vector3 v2(4.0, 2.5, -1.0);
    Vector3 vm = v1.midPointTo(v2);
    EXPECT_VECTOR3_COORDS(vm, 2.5, 2.25, 3.5);
}

TEST(Vector3, randomInSphere) {
    Vector3 v;

    v = Vector3::randomInSphere();
    EXPECT_LE(v.getNorm(), 1.0);

    v = Vector3::randomInSphere(0.01);
    EXPECT_LE(v.getNorm(), 0.01);

    v = Vector3::randomInSphere(0.5, true);
    EXPECT_DOUBLE_EQ(v.getNorm(), 0.5);
}

TEST(Vector3, getNormal3) {
    EXPECT_VECTOR3_COORDS(VECTOR_ZERO.getNormal3(VECTOR_SOUTH, VECTOR_EAST), 0.0, 1.0, 0.0);
}

TEST(Vector3, getNormal5) {
    EXPECT_VECTOR3_COORDS(VECTOR_ZERO.getNormal5(VECTOR_SOUTH, VECTOR_NORTH, VECTOR_EAST, VECTOR_WEST), 0.0, 1.0, 0.0);
}
