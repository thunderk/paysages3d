#include "BaseTestCase.h"
#include "Vector3.h"

TEST(Vector3, midPointTo)
{
    Vector3 v1(1.0, 2.0, 8.0);
    Vector3 v2(4.0, 2.5, -1.0);
    Vector3 vm = v1.midPointTo(v2);
    EXPECT_VECTOR3_COORDS(vm, 2.5, 2.25, 3.5);
}
