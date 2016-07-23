#include "BaseTestCase.h"

#include "DefinitionNode.h"
#include "DefinitionWatcher.h"
#include "DiffManager.h"
#include "FloatDiff.h"
#include "FloatNode.h"
#include "TestToolDefinition.h"

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

namespace {
class TestWatcher : public DefinitionWatcher {
  public:
    TestWatcher(DefinitionNode *expected_node, double expected_old_value, double expected_new_value)
        : DefinitionWatcher("TestWatcher"), expected_node(expected_node), expected_old_value(expected_old_value),
          expected_new_value(expected_new_value) {
        calls = 0;
    }

    void start(bool init_diffs) {
        startWatching(expected_node, init_diffs);
    }

    virtual void nodeChanged(const DefinitionNode *node, const DefinitionDiff *diff, const DefinitionNode *) override {
        EXPECT_EQ(expected_node, node);
        ASSERT_EQ("float", diff->getTypeName());
        auto float_diff = static_cast<const FloatDiff *>(diff);
        EXPECT_EQ(expected_old_value, float_diff->getOldValue());
        EXPECT_EQ(expected_new_value, float_diff->getNewValue());
        calls++;
    }

    int calls;
    DefinitionNode *expected_node;
    double expected_old_value;
    double expected_new_value;
};
}

TEST(DiffManager, addWatcher) {
    FloatNode node(NULL, "node");
    TestWatcher watcher(&node, 1.3, -4.0);

    node.setValue(1.3);
    EXPECT_EQ(0, watcher.calls);

    watcher.start(false);
    EXPECT_EQ(0, watcher.calls);

    node.setValue(-4.0);
    EXPECT_EQ(1, watcher.calls);
}

TEST(DiffManager, addWatcherWithInitDiffs) {
    FloatNode node(NULL, "node", 1.3);
    TestWatcher watcher(&node, 1.3, 1.3);

    watcher.start(true);
    EXPECT_EQ(1, watcher.calls);
}

TEST(DiffManager, publishToWatcher) {
    DefinitionNode root(NULL, "root");
    FloatNode node(&root, "node", 1.3);
    RecordingDefinitionWatcher watcher;

    EXPECT_EQ(0u, watcher.changes.size());

    watcher.start(&root);
    ASSERT_EQ(1u, watcher.changes.size());
    EXPECT_EQ(&root, watcher.changes[0].node);
    EXPECT_EQ(&root, watcher.changes[0].parent);

    watcher.start(&node);
    ASSERT_EQ(2u, watcher.changes.size());
    EXPECT_EQ(&node, watcher.changes[1].node);
    EXPECT_EQ(&node, watcher.changes[1].parent);

    node.setValue(2.3);
    ASSERT_EQ(4u, watcher.changes.size());
    EXPECT_EQ(&node, watcher.changes[2].node);
    EXPECT_EQ(&node, watcher.changes[2].parent);
    EXPECT_EQ(&node, watcher.changes[3].node);
    EXPECT_EQ(&root, watcher.changes[3].parent);
}
