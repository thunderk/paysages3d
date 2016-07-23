#include "BaseTestCase.h"

#include "ColorDiff.h"
#include "ColorNode.h"
#include "IntDiff.h"
#include "PackStream.h"

TEST(ColorNode, toString) {
    ColorNode test(NULL, "test", Color(0.1, 0.2, 0.3, 0.4));

    EXPECT_EQ("test 0.1,0.2,0.3,0.4", test.toString(0));
    EXPECT_EQ("  test 0.1,0.2,0.3,0.4", test.toString(2));
}

TEST(ColorNode, saveLoadAndSkip) {
    DefinitionNode root1(NULL, "root");
    ColorNode testa1(&root1, "testa", COLOR_RED);
    ColorNode testb1(&root1, "testb", COLOR_GREEN);

    PackStream stream1;
    root1.save(&stream1);

    DefinitionNode root2(NULL, "root");
    ColorNode testb2(&root2, "testb");

    PackStream stream2(&stream1);
    root2.load(&stream2);

    EXPECT_EQ(COLOR_GREEN, testb2.getValue());
}

TEST(ColorNode, copy) {
    ColorNode base(NULL, "test", COLOR_BLUE);
    ColorNode other(NULL, "test", COLOR_RED);
    DefinitionNode badother(NULL, "test");

    base.copy(&other);
    EXPECT_EQ(COLOR_BLUE, base.getValue());
    EXPECT_EQ(COLOR_BLUE, other.getValue());

    badother.copy(&base);
    EXPECT_EQ(COLOR_BLUE, base.getValue());

    base.copy(&badother);
    // can't check anything, just useful in valgrind
}

TEST(ColorNode, produceDiff) {
    ColorNode node(NULL, "test", COLOR_BLUE);
    const ColorDiff *diff = node.produceDiff(COLOR_RED);

    EXPECT_EQ("color", diff->getTypeName());
    EXPECT_EQ(COLOR_BLUE, diff->getOldValue());
    EXPECT_EQ(COLOR_RED, diff->getNewValue());

    bool result = node.applyDiff(diff);

    EXPECT_TRUE(result);
    EXPECT_EQ(COLOR_RED, node.getValue());

    delete diff;
}

TEST(ColorNode, applyDiff) {
    ColorNode node(NULL, "test", COLOR_BLUE);
    ColorDiff diff(&node, COLOR_BLUE, COLOR_RED);
    DefinitionNode onode(NULL, "test", "badtype");
    IntDiff odiff(&onode, 1, 2);
    bool result;

    EXPECT_EQ(COLOR_BLUE, node.getValue());

    // apply
    result = node.applyDiff(&diff);

    EXPECT_TRUE(result);
    EXPECT_EQ(COLOR_RED, node.getValue());

    // can't apply twice
    result = node.applyDiff(&diff);

    EXPECT_FALSE(result);
    EXPECT_EQ(COLOR_RED, node.getValue());

    // reverse
    result = node.applyDiff(&diff, true);

    EXPECT_TRUE(result);
    EXPECT_EQ(COLOR_BLUE, node.getValue());

    // can't reverse twice
    result = node.applyDiff(&diff, true);

    EXPECT_FALSE(result);
    EXPECT_EQ(COLOR_BLUE, node.getValue());

    // bad type
    result = node.applyDiff(&odiff);

    EXPECT_FALSE(result);
    EXPECT_EQ(COLOR_BLUE, node.getValue());

    // apply again
    result = node.applyDiff(&diff);

    EXPECT_TRUE(result);
    EXPECT_EQ(COLOR_RED, node.getValue());
}
