#include "BaseTestCase.h"

#include "DiffManager.h"
#include "DefinitionNode.h"
#include "FloatNode.h"

TEST(DiffManager, undoRedo)
{
    DefinitionNode root(NULL, "root");
    DiffManager *diffs = root.getDiffManager();

    FloatNode leaf(&root, "value", 2.6);
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

TEST(DiffManager, undoTooMuch)
{
    DefinitionNode root(NULL, "root");
    DiffManager *diffs = root.getDiffManager();

    FloatNode leaf(&root, "value", 1.1);
    EXPECT_DOUBLE_EQ(1.1, leaf.getValue());

    leaf.setValue(2.2);
    EXPECT_DOUBLE_EQ(2.2, leaf.getValue());

    diffs->undo();
    EXPECT_DOUBLE_EQ(1.1, leaf.getValue());

    diffs->undo();
    EXPECT_DOUBLE_EQ(1.1, leaf.getValue());

    diffs->redo();
    EXPECT_DOUBLE_EQ(2.2, leaf.getValue());
}

TEST(DiffManager, redoTooMuch)
{
    DefinitionNode root(NULL, "root");
    DiffManager *diffs = root.getDiffManager();

    FloatNode leaf(&root, "value", 1.1);
    EXPECT_DOUBLE_EQ(1.1, leaf.getValue());

    leaf.setValue(2.2);
    EXPECT_DOUBLE_EQ(2.2, leaf.getValue());

    diffs->undo();
    EXPECT_DOUBLE_EQ(1.1, leaf.getValue());

    diffs->redo();
    EXPECT_DOUBLE_EQ(2.2, leaf.getValue());

    diffs->redo();
    EXPECT_DOUBLE_EQ(2.2, leaf.getValue());

    diffs->undo();
    EXPECT_DOUBLE_EQ(1.1, leaf.getValue());
}

TEST(DiffManager, undoBranch)
{
    DefinitionNode root(NULL, "root");
    DiffManager *diffs = root.getDiffManager();

    FloatNode leaf(&root, "value", 1.1);
    EXPECT_DOUBLE_EQ(1.1, leaf.getValue());

    leaf.setValue(2.2);
    EXPECT_DOUBLE_EQ(2.2, leaf.getValue());

    leaf.setValue(3.3);
    EXPECT_DOUBLE_EQ(3.3, leaf.getValue());

    diffs->undo();
    EXPECT_DOUBLE_EQ(2.2, leaf.getValue());

    leaf.setValue(4.4);
    EXPECT_DOUBLE_EQ(4.4, leaf.getValue());

    diffs->undo();
    EXPECT_DOUBLE_EQ(2.2, leaf.getValue());

    diffs->undo();
    EXPECT_DOUBLE_EQ(1.1, leaf.getValue());

    diffs->redo();
    EXPECT_DOUBLE_EQ(2.2, leaf.getValue());

    diffs->redo();
    EXPECT_DOUBLE_EQ(4.4, leaf.getValue());

    diffs->redo();
    EXPECT_DOUBLE_EQ(4.4, leaf.getValue());
}
