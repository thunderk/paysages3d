#include "BaseTestCase.h"

#include "DefinitionNode.h"

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
