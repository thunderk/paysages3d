#include "BaseTestCase.h"

#include "FloatNode.h"
#include "FloatDiff.h"
#include "PackStream.h"

TEST(FloatNode, toString)
{
    FloatNode test(NULL, "test", 2.1);

    EXPECT_EQ("test 2.1", test.toString(0));
    EXPECT_EQ("  test 2.1", test.toString(2));
}

TEST(FloatNode, saveLoadAndSkip)
{
    DefinitionNode root1(NULL, "root");
    FloatNode testa1(&root1, "testa", 1.5);
    FloatNode testb1(&root1, "testb", 4.3);

    PackStream stream1;
    root1.save(&stream1);

    DefinitionNode root2(NULL, "root");
    FloatNode testb2(&root2, "testb");

    PackStream stream2(&stream1);
    root2.load(&stream2);

    EXPECT_DOUBLE_EQ(4.3, testb2.getValue());
}

TEST(FloatNode, copy)
{
    FloatNode base(NULL, "test", 2.1);
    FloatNode other(NULL, "test", 4.3);
    DefinitionNode badother(NULL, "test");

    base.copy(&other);
    EXPECT_DOUBLE_EQ(2.1, base.getValue());
    EXPECT_DOUBLE_EQ(2.1, other.getValue());

    badother.copy(&base);
    EXPECT_DOUBLE_EQ(2.1, base.getValue());

    base.copy(&badother);
    // can't check anything, just useful in valgrind
}

TEST(FloatNode, produceDiff)
{
    FloatNode node(NULL, "test", 8.3);
    const FloatDiff *diff = node.produceDiff(-4.1);

    EXPECT_EQ("float", diff->getTypeName());
    EXPECT_DOUBLE_EQ(8.3, diff->getOldValue());
    EXPECT_DOUBLE_EQ(-4.1, diff->getNewValue());

    bool result = node.applyDiff(diff);

    EXPECT_TRUE(result);
    EXPECT_DOUBLE_EQ(-4.1, node.getValue());

    delete diff;
}

TEST(FloatNode, applyDiff)
{
    FloatNode node(NULL, "test", 1.2);
    FloatDiff diff(1.2, 2.4);
    DefinitionDiff odiff("badtype");
    bool result;

    EXPECT_DOUBLE_EQ(1.2, node.getValue());

    // apply
    result = node.applyDiff(&diff);

    EXPECT_TRUE(result);
    EXPECT_DOUBLE_EQ(2.4, node.getValue());

    // can't apply twice
    result = node.applyDiff(&diff);

    EXPECT_FALSE(result);
    EXPECT_DOUBLE_EQ(2.4, node.getValue());

    // reverse
    result = node.applyDiff(&diff, true);

    EXPECT_TRUE(result);
    EXPECT_DOUBLE_EQ(1.2, node.getValue());

    // can't reverse twice
    result = node.applyDiff(&diff, true);

    EXPECT_FALSE(result);
    EXPECT_DOUBLE_EQ(1.2, node.getValue());

    // bad type
    result = node.applyDiff(&odiff);

    EXPECT_FALSE(result);
    EXPECT_DOUBLE_EQ(1.2, node.getValue());

    // apply again
    result = node.applyDiff(&diff);

    EXPECT_TRUE(result);
    EXPECT_DOUBLE_EQ(2.4, node.getValue());
}
