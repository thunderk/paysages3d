#ifndef TESTTOOLDEFINITION_H
#define TESTTOOLDEFINITION_H

#include "DefinitionWatcher.h"

#include <vector>

namespace {
/**
 * Definition watcher that registers all calls to nodeChanged.
 */
class RecordingDefinitionWatcher : public DefinitionWatcher {
  public:
    RecordingDefinitionWatcher() {
    }

    virtual void nodeChanged(const DefinitionNode *node, const DefinitionDiff *diff, const DefinitionNode *parent) override {
        RecordedChange change;
        change.node = node;
        change.diff = diff;  // FIXME Referenced diff may get deleted by the diff manager
        change.parent = parent;
        changes.push_back(change);
    }

    typedef struct {
        const DefinitionNode *node;
        const DefinitionDiff *diff;
        const DefinitionNode *parent;
    } RecordedChange;
    vector<RecordedChange> changes;
};
}


#endif // TESTTOOLDEFINITION_H
