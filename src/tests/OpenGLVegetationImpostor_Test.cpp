#include "BaseTestCase.h"
#include "OpenGLVegetationImpostor.h"

#include "Vector3.h"

TEST(OpenGLVegetationImpostor, getIndex) {
    OpenGLVegetationImpostor impostor;

    EXPECT_EQ(0, impostor.getIndex(Vector3(0.0, 1.0, 0.0), VECTOR_ZERO));

    EXPECT_EQ(1, impostor.getIndex(Vector3(1.0, 1.0, 0.0), VECTOR_ZERO));
    EXPECT_EQ(1, impostor.getIndex(Vector3(1.0, 1.0, 0.1), VECTOR_ZERO));
    EXPECT_EQ(1, impostor.getIndex(Vector3(1.0, 1.0, -0.1), VECTOR_ZERO));

    EXPECT_EQ(2, impostor.getIndex(Vector3(1.0, 1.0, -1.0), VECTOR_ZERO));
    EXPECT_EQ(5, impostor.getIndex(Vector3(1.0, 1.0, 1.0), VECTOR_ZERO));

    EXPECT_EQ(6, impostor.getIndex(Vector3(1.0, 0.0, 0.0), VECTOR_ZERO));
    EXPECT_EQ(6, impostor.getIndex(Vector3(1.0, 0.0, 0.1), VECTOR_ZERO));
    EXPECT_EQ(6, impostor.getIndex(Vector3(1.0, 0.0, -0.1), VECTOR_ZERO));
}
