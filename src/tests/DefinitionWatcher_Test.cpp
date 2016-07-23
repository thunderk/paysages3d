#include "DefinitionWatcher.h"
#include "BaseTestCase.h"

#include "IntNode.h"

TEST(DefinitionWatcher, destructor) {
    class FakeWatcher : public DefinitionWatcher {
      public:
        FakeWatcher(DefinitionNode *node, int &counter) : DefinitionWatcher("FakeWatcher"), counter(counter) {
            startWatching(node, false);
        }

        virtual void nodeChanged(const DefinitionNode *, const DefinitionDiff *, const DefinitionNode *) override {
            counter++;
        }

        int &counter;
    };

    IntNode root(NULL, "test", 5);
    int counter = 0;
    auto watcher = new FakeWatcher(&root, counter);

    EXPECT_EQ(0, counter);

    root.setValue(3);
    EXPECT_EQ(1, counter);

    // When the watcher is destroyed, it should stop receiving diffs
    delete watcher;
    root.setValue(8);
    EXPECT_EQ(1, counter);
}
