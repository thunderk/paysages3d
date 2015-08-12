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
