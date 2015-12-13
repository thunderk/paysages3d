#include "BaseTestCase.h"
#include "OpenGLTerrainChunk.h"

#include "Scenery.h"
#include "OpenGLRenderer.h"
#include "OpenGLVertexArray.h"
#include "Vector3.h"

static void checkVertex(const OpenGLVertexArray *array, int index, const Vector3 &expected_location, double expected_u,
                        double expected_v) {
    Vector3 location;
    double u, v;

    array->get(index, &location, &u, &v);

    EXPECT_VECTOR3_COORDS(location, expected_location.x, expected_location.y, expected_location.z);
    EXPECT_DOUBLE_EQ(expected_u, u);
    EXPECT_DOUBLE_EQ(expected_v, v);
}

TEST(OpenGLTerrainChunk, setFirstStepVertices) {
    Scenery scenery;
    OpenGLRenderer renderer(&scenery);
    OpenGLTerrainChunk chunk(&renderer, 0.0, 0.0, 1.0, 1);

    ASSERT_EQ(0, chunk.getVerticesLevel());

    chunk.setFirstStepVertices();

    EXPECT_EQ(1, chunk.getVerticesLevel());
    ASSERT_EQ(6, chunk.getVertices()->getVertexCount());
    const OpenGLVertexArray *array = chunk.getVertices();
    checkVertex(array, 0, Vector3(0.0, 0.0, 0.0), 0.0, 0.0);
    checkVertex(array, 1, Vector3(0.0, 0.0, 1.0), 0.0, 1.0);
    checkVertex(array, 2, Vector3(1.0, 0.0, 0.0), 1.0, 0.0);
    checkVertex(array, 3, Vector3(1.0, 0.0, 1.0), 1.0, 1.0);
    checkVertex(array, 4, Vector3(1.0, 0.0, 0.0), 1.0, 0.0);
    checkVertex(array, 5, Vector3(0.0, 0.0, 1.0), 0.0, 1.0);
}

TEST(OpenGLTerrainChunk, augmentVertices) {
    Scenery scenery;
    OpenGLRenderer renderer(&scenery);
    OpenGLTerrainChunk chunk(&renderer, 0.0, 0.0, 1.0, 1);

    ASSERT_EQ(0, chunk.getVerticesLevel());

    chunk.setFirstStepVertices();

    ASSERT_EQ(1, chunk.getVerticesLevel());

    chunk.augmentVertices();

    ASSERT_EQ(2, chunk.getVerticesLevel());
    EXPECT_EQ(24, chunk.getVertices()->getVertexCount());
    const OpenGLVertexArray *array = chunk.getVertices();
    checkVertex(array, 0, Vector3(0.0, 0.0, 0.0), 0.0, 0.0);
    checkVertex(array, 1, Vector3(0.0, 0.0, 0.5), 0.0, 0.5);
    checkVertex(array, 2, Vector3(0.5, 0.0, 0.0), 0.5, 0.0);
    checkVertex(array, 3, Vector3(0.5, 0.0, 0.5), 0.5, 0.5);
    checkVertex(array, 4, Vector3(0.5, 0.0, 0.0), 0.5, 0.0);
    checkVertex(array, 5, Vector3(0.0, 0.0, 0.5), 0.0, 0.5);
    checkVertex(array, 6, Vector3(0.5, 0.0, 0.0), 0.5, 0.0);
    checkVertex(array, 7, Vector3(0.5, 0.0, 0.5), 0.5, 0.5);
    checkVertex(array, 8, Vector3(1.0, 0.0, 0.0), 1.0, 0.0);
    checkVertex(array, 9, Vector3(1.0, 0.0, 0.5), 1.0, 0.5);
    checkVertex(array, 10, Vector3(1.0, 0.0, 0.0), 1.0, 0.0);
    checkVertex(array, 11, Vector3(0.5, 0.0, 0.5), 0.5, 0.5);
}
