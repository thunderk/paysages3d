#include "BaseTestCase.h"

#include "DiffManager.h"
#include "DefinitionNode.h"
#include "DefinitionWatcher.h"
#include "FloatDiff.h"
#include "FloatNode.h"

TEST(DiffManager, undoRedo) {
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

TEST(DiffManager, undoTooMuch) {
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

TEST(DiffManager, redoTooMuch) {
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

TEST(DiffManager, undoBranch) {
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

class TestWatcher : public DefinitionWatcher {
  public:
    TestWatcher(DefinitionNode *expected_node, double expected_old_value, double expected_new_value)
        : DefinitionWatcher(), expected_node(expected_node), expected_old_value(expected_old_value),
          expected_new_value(expected_new_value) {
        calls = 0;
    }

    virtual void nodeChanged(const DefinitionNode *node, const DefinitionDiff *diff) override {
        EXPECT_EQ(expected_node, node);
        ASSERT_EQ("float", diff->getTypeName());
        const FloatDiff *float_diff = (const FloatDiff *)diff;
        EXPECT_EQ(expected_old_value, float_diff->getOldValue());
        EXPECT_EQ(expected_new_value, float_diff->getNewValue());
        calls++;
    }

    int calls;
    DefinitionNode *expected_node;
    double expected_old_value;
    double expected_new_value;
};

TEST(DiffManager, addWatcher) {
    FloatNode node(NULL, "node");
    TestWatcher watcher(&node, 1.3, -4.0);

    node.setValue(1.3);
    EXPECT_EQ(0, watcher.calls);

    node.addWatcher(&watcher);
    EXPECT_EQ(0, watcher.calls);

    node.setValue(-4.0);
    EXPECT_EQ(1, watcher.calls);
}

TEST(DiffManager, addWatcherWithInitDiffs) {
    FloatNode node(NULL, "node", 1.3);
    TestWatcher watcher(&node, 1.3, 1.3);

    node.addWatcher(&watcher, true);
    EXPECT_EQ(1, watcher.calls);
}
