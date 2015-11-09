#include "BaseTestCase.h"

#include "IntNode.h"
#include "IntDiff.h"
#include "PackStream.h"

TEST(IntNode, toString) {
    IntNode test(NULL, "test", 2);

    EXPECT_EQ("test 2", test.toString(0));
    EXPECT_EQ("  test 2", test.toString(2));
}

TEST(IntNode, saveLoadAndSkip) {
    DefinitionNode root1(NULL, "root");
    IntNode testa1(&root1, "testa", 1);
    IntNode testb1(&root1, "testb", 4);

    PackStream stream1;
    root1.save(&stream1);

    DefinitionNode root2(NULL, "root");
    IntNode testb2(&root2, "testb");

    PackStream stream2(&stream1);
    root2.load(&stream2);

    EXPECT_DOUBLE_EQ(4, testb2.getValue());
}

TEST(IntNode, copy) {
    IntNode base(NULL, "test", 2);
    IntNode other(NULL, "test", 4);
    DefinitionNode badother(NULL, "test");

    base.copy(&other);
    EXPECT_DOUBLE_EQ(2, base.getValue());
    EXPECT_DOUBLE_EQ(2, other.getValue());

    badother.copy(&base);
    EXPECT_DOUBLE_EQ(2, base.getValue());

    base.copy(&badother);
    // can't check anything, just useful in valgrind
}

TEST(IntNode, produceDiff) {
    IntNode node(NULL, "test", 8);
    const IntDiff *diff = node.produceDiff(-4);

    EXPECT_EQ("int", diff->getTypeName());
    EXPECT_DOUBLE_EQ(8, diff->getOldValue());
    EXPECT_DOUBLE_EQ(-4, diff->getNewValue());

    bool result = node.applyDiff(diff);

    EXPECT_TRUE(result);
    EXPECT_DOUBLE_EQ(-4, node.getValue());

    delete diff;
}

TEST(IntNode, applyDiff) {
    IntNode node(NULL, "test", 1);
    IntDiff diff(&node, 1, 2);
    DefinitionNode onode(NULL, "test", "badtype");
    DefinitionDiff odiff(&onode);
    bool result;

    EXPECT_DOUBLE_EQ(1, node.getValue());

    // apply
    result = node.applyDiff(&diff);

    EXPECT_TRUE(result);
    EXPECT_DOUBLE_EQ(2, node.getValue());

    // can't apply twice
    result = node.applyDiff(&diff);

    EXPECT_FALSE(result);
    EXPECT_DOUBLE_EQ(2, node.getValue());

    // reverse
    result = node.applyDiff(&diff, true);

    EXPECT_TRUE(result);
    EXPECT_DOUBLE_EQ(1, node.getValue());

    // can't reverse twice
    result = node.applyDiff(&diff, true);

    EXPECT_FALSE(result);
    EXPECT_DOUBLE_EQ(1, node.getValue());

    // bad type
    result = node.applyDiff(&odiff);

    EXPECT_FALSE(result);
    EXPECT_DOUBLE_EQ(1, node.getValue());

    // apply again
    result = node.applyDiff(&diff);

    EXPECT_TRUE(result);
    EXPECT_DOUBLE_EQ(2, node.getValue());
}
