#include "BaseTestCase.h"

#include "VertexArray.h"

class TestVertex {
  public:
    float uv[2];
    int loc[3];

    bool operator==(const TestVertex &other) const {
        return other.uv[0] == uv[0] and other.uv[1] == uv[1] and other.loc[0] == loc[0] and other.loc[1] == loc[1] and
               other.loc[2] == loc[2];
    }
};

TEST(VertexArray, grid) {
    VertexArray<TestVertex> array;

    array.setGridSize(3);

    ASSERT_EQ(9, array.getVertexCount());

    TestVertex v1 = {{0.1, 0.2}, {1, 2, 3}};
    TestVertex vgot;

    array.setGridVertex(3, 1, 2, v1);
    vgot = array.getGridVertex(3, 1, 2);
    EXPECT_EQ(v1, vgot);
    vgot = array.getVertex(7);
    EXPECT_EQ(v1, vgot);
}

TEST(VertexArray, gridIndices) {
    VertexArray<TestVertex> array;

    array.setGridSize(3);

    array.setAutoGridIndices(3);
    ASSERT_EQ(24, array.getIndexCount());

    EXPECT_EQ(0, array.getIndex(0));
    EXPECT_EQ(3, array.getIndex(1));
    EXPECT_EQ(1, array.getIndex(2));

    EXPECT_EQ(1, array.getIndex(3));
    EXPECT_EQ(3, array.getIndex(4));
    EXPECT_EQ(4, array.getIndex(5));

    EXPECT_EQ(1, array.getIndex(6));
    EXPECT_EQ(4, array.getIndex(7));
    EXPECT_EQ(2, array.getIndex(8));

    EXPECT_EQ(2, array.getIndex(9));
    EXPECT_EQ(4, array.getIndex(10));
    EXPECT_EQ(5, array.getIndex(11));

    EXPECT_EQ(3, array.getIndex(12));
    EXPECT_EQ(6, array.getIndex(13));
    EXPECT_EQ(4, array.getIndex(14));
}

TEST(VertexArray, gridIndicesStride) {
    VertexArray<TestVertex> array;

    array.setGridSize(5);

    array.setAutoGridIndices(5, 1);
    ASSERT_EQ(96, array.getIndexCount());

    array.setAutoGridIndices(5, 2);
    ASSERT_EQ(24, array.getIndexCount());

    EXPECT_EQ(0, array.getIndex(0));
    EXPECT_EQ(10, array.getIndex(1));
    EXPECT_EQ(2, array.getIndex(2));

    EXPECT_EQ(2, array.getIndex(3));
    EXPECT_EQ(10, array.getIndex(4));
    EXPECT_EQ(12, array.getIndex(5));

    EXPECT_EQ(2, array.getIndex(6));
    EXPECT_EQ(12, array.getIndex(7));
    EXPECT_EQ(4, array.getIndex(8));

    EXPECT_EQ(4, array.getIndex(9));
    EXPECT_EQ(12, array.getIndex(10));
    EXPECT_EQ(14, array.getIndex(11));

    EXPECT_EQ(10, array.getIndex(12));
    EXPECT_EQ(20, array.getIndex(13));
    EXPECT_EQ(12, array.getIndex(14));
}
