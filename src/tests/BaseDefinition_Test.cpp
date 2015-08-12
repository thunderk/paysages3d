#include "BaseTestCase.h"

#include "BaseDefinition.h"

TEST(BaseDefinition, toString)
{
    BaseDefinition root(NULL, "root");
    BaseDefinition branch(&root, "branch");
    BaseDefinition leaf1(&branch, "leaf1");
    BaseDefinition leaf2(&branch, "leaf2");

    EXPECT_EQ("root\n branch\n  leaf1\n  leaf2", root.toString());
    EXPECT_EQ("branch\n leaf1\n leaf2", branch.toString());
}

TEST(BaseDefinition, attachDetach)
{
    BaseDefinition* root = new BaseDefinition(NULL, "root");
    BaseDefinition* child1 = new BaseDefinition(root, "child1");
    BaseDefinition* child2 = new BaseDefinition(root, "child2");

    EXPECT_EQ(root, child1->getParent());
    EXPECT_EQ(root, child2->getParent());
    EXPECT_EQ(2, root->getChildrenCount());

    delete child1;

    EXPECT_EQ(1, root->getChildrenCount());

    delete child2;

    EXPECT_EQ(0, root->getChildrenCount());

    delete root;
}
