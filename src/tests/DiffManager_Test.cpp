#include "BaseTestCase.h"

#include "DiffManager.h"
#include "DefinitionNode.h"
#include "FloatNode.h"

TEST(DiffManager, undoRedoSimple)
{
    DefinitionNode root(NULL, "root");
    FloatNode leaf(&root, "value", 2.6);
    DiffManager *diffs = root.getDiffManager();

    EXPECT_DOUBLE_EQ(2.6, leaf.getValue());

    leaf.setValue(4.3);
    EXPECT_DOUBLE_EQ(4.3, leaf.getValue());

    leaf.setValue(-2.1);
    EXPECT_DOUBLE_EQ(-2.1, leaf.getValue());

    diffs->undo();
    EXPECT_DOUBLE_EQ(4.3, leaf.getValue());

    diffs->undo();
    EXPECT_DOUBLE_EQ(2.6, leaf.getValue());

    diffs->redo();
    EXPECT_DOUBLE_EQ(4.3, leaf.getValue());

    diffs->redo();
    EXPECT_DOUBLE_EQ(-2.1, leaf.getValue());
}
