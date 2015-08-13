#include "BaseTestCase.h"

#include "DefinitionNode.h"
#include "FloatNode.h"
#include "PackStream.h"

TEST(DefinitionNode, toString)
{
    DefinitionNode root(NULL, "root");
    DefinitionNode branch(&root, "branch");
    DefinitionNode leaf1(&branch, "leaf1");
    DefinitionNode leaf2(&branch, "leaf2");

    EXPECT_EQ("root\n branch\n  leaf1\n  leaf2", root.toString());
    EXPECT_EQ("branch\n leaf1\n leaf2", branch.toString());
}

TEST(DefinitionNode, attachDetach)
{
    DefinitionNode* root = new DefinitionNode(NULL, "root");
    DefinitionNode* child1 = new DefinitionNode(root, "child1");
    DefinitionNode* child2 = new DefinitionNode(root, "child2");

    EXPECT_EQ(root, child1->getParent());
    EXPECT_EQ(root, child2->getParent());
    EXPECT_EQ(2, root->getChildrenCount());

    delete child1;

    EXPECT_EQ(1, root->getChildrenCount());

    delete child2;

    EXPECT_EQ(0, root->getChildrenCount());

    delete root;
}

TEST(DefinitionNode, saveLoad)
{
    PackStream *stream;

    DefinitionNode* before = new DefinitionNode(NULL, "root");
    DefinitionNode* before1 = new DefinitionNode(before, "before1");
    new FloatNode(before1, "before11", 2.1);
    new FloatNode(before1, "before12", -4.3);
    DefinitionNode* before2 = new DefinitionNode(before, "before2");
    new DefinitionNode(before2, "before21");
    new FloatNode(before2, "before22");
    new FloatNode(before, "before3", 6.7);

    stream = new PackStream();
    stream->bindToFile("/tmp/test_paysages_pack", true);
    before->save(stream);
    delete stream;

    // Same definition tree, but with missing nodes, and added ones
    DefinitionNode* after = new DefinitionNode(NULL, "root");
    DefinitionNode* after1 = new DefinitionNode(after, "before1");
    FloatNode* after12 = new FloatNode(after1, "before12");
    FloatNode* after13 = new FloatNode(after1, "before13");
    FloatNode* after3 = new FloatNode(after, "before3");
    FloatNode* after4 = new FloatNode(after, "before4");

    stream = new PackStream();
    stream->bindToFile("/tmp/test_paysages_pack");
    after->load(stream);
    delete stream;

    EXPECT_DOUBLE_EQ(-4.3, after12->getValue());
    EXPECT_DOUBLE_EQ(0.0, after13->getValue());
    EXPECT_DOUBLE_EQ(6.7, after3->getValue());
    EXPECT_DOUBLE_EQ(0.0, after4->getValue());

    delete before;
    delete after;
}
