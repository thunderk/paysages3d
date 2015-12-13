#include "DiffManager.h"

#include <algorithm>
#include "DefinitionNode.h"
#include "DefinitionDiff.h"
#include "DefinitionWatcher.h"
#include "Logs.h"

DiffManager::DiffManager(DefinitionNode *tree) : tree(tree) {
    undone = 0;
}

DiffManager::~DiffManager() {
    for (auto diff : diffs) {
        delete diff;
    }
    diffs.clear();
}

void DiffManager::addWatcher(const DefinitionNode *node, DefinitionWatcher *watcher) {
    if (find(watchers[node].begin(), watchers[node].end(), watcher) == watchers[node].end()) {
        watchers[node].push_back(watcher);
    }
}

int DiffManager::getWatcherCount(const DefinitionNode *node) {
    return watchers[node].size();
}

void DiffManager::addDiff(DefinitionNode *node, const DefinitionDiff *diff) {
    while (undone > 0) {
        // truncate diffs ahead
        delete diffs.back();
        diffs.pop_back();
        undone--;
    }

    diffs.push_back(diff);

    // TODO Delayed commit (with merge of consecutive diffs)
    node->applyDiff(diff);

    for (auto watcher : watchers[node]) {
        watcher->nodeChanged(node, diff);
    }
}

void DiffManager::undo() {
    if (undone < (int)diffs.size()) {
        const DefinitionDiff *diff = diffs[diffs.size() - undone - 1];

        // Obtain the node by path and reverse apply diff on it
        DefinitionNode *node = tree->findByPath(diff->getPath());
        if (node) {
            undone++;

            node->applyDiff(diff, true);

            for (auto watcher : watchers[node]) {
                // FIXME Reverse diff
                watcher->nodeChanged(node, diff);
            }
        } else {
            Logs::error("Definition") << "Can't find node to undo diff : " << diff->getPath() << endl;
        }
    }
}

void DiffManager::redo() {
    if (undone > 0) {
        const DefinitionDiff *diff = diffs[diffs.size() - undone];

        // Obtain the node by path and re-apply diff on it
        DefinitionNode *node = tree->findByPath(diff->getPath());
        if (node) {
            undone--;

            node->applyDiff(diff);

            for (auto watcher : watchers[node]) {
                watcher->nodeChanged(node, diff);
            }
        } else {
            Logs::error("Definition") << "Can't find node to redo diff : " << diff->getPath() << endl;
        }
    }
}
