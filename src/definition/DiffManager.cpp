#include "DiffManager.h"

#include "DefinitionDiff.h"
#include "DefinitionNode.h"
#include "DefinitionWatcher.h"
#include "Logs.h"
#include <algorithm>
#include <map>
#include <mutex>
#include <vector>

class DiffManager::pimpl {
  public:
    pimpl(DefinitionNode *tree) : tree(tree), undone(0) {
    }

    recursive_mutex lock;
    DefinitionNode *tree;
    unsigned long undone;
    vector<const DefinitionDiff *> diffs;
    map<const DefinitionNode *, vector<DefinitionWatcher *>> watchers;
};

DiffManager::DiffManager(DefinitionNode *tree) : impl(new pimpl(tree)) {
}

DiffManager::~DiffManager() {
    // TODO smart pointers
    for (auto diff : impl->diffs) {
        delete diff;
    }
    impl->diffs.clear();
}

unsigned long DiffManager::getDiffCount(int include_undone) const {
    return include_undone ? impl->diffs.size() : impl->diffs.size() - impl->undone;
}

void DiffManager::addWatcher(const DefinitionNode *node, DefinitionWatcher *watcher) {
    impl->lock.lock();
    auto &watchers = impl->watchers[node];
    if (find(watchers.begin(), watchers.end(), watcher) == watchers.end()) {
        watchers.push_back(watcher);
    }
    impl->lock.unlock();
}

void DiffManager::removeWatcher(DefinitionWatcher *watcher) {
    impl->lock.lock();
    for (auto &item : impl->watchers) {
        auto &node_watchers = item.second;
        node_watchers.erase(remove(node_watchers.begin(), node_watchers.end(), watcher), node_watchers.end());
    }
    impl->lock.unlock();
}

unsigned long DiffManager::getWatcherCount(const DefinitionNode *node) {
    return impl->watchers[node].size();
}

void DiffManager::addDiff(DefinitionNode *node, const DefinitionDiff *diff) {
    while (impl->undone > 0) {
        // truncate diffs ahead
        delete impl->diffs.back();
        impl->diffs.pop_back();
        impl->undone--;
    }

    impl->diffs.push_back(diff);

    // TODO Delayed commit (with merge of consecutive diffs)

    Logs::debug("Definition") << "Node changed : " << node->getPath() << endl;
    node->applyDiff(diff);
    publishToWatchers(node, diff);
}

void DiffManager::undo() {
    if (impl->undone < impl->diffs.size()) {
        const DefinitionDiff *diff = impl->diffs[impl->diffs.size() - impl->undone - 1];

        // Obtain the node by path and reverse apply diff on it
        DefinitionNode *node = impl->tree->findByPath(diff->getPath());
        if (node) {
            impl->undone++;

            unique_ptr<DefinitionDiff> reversed(diff->newReversed());

            Logs::debug("Definition") << "Node undo : " << node->getPath() << endl;
            // TODO use reversed ?
            node->applyDiff(diff, true);
            publishToWatchers(node, reversed.get());
        } else {
            Logs::error("Definition") << "Can't find node to undo diff : " << diff->getPath() << endl;
        }
    }
}

void DiffManager::redo() {
    if (impl->undone > 0) {
        const DefinitionDiff *diff = impl->diffs[impl->diffs.size() - impl->undone];

        // Obtain the node by path and re-apply diff on it
        DefinitionNode *node = impl->tree->findByPath(diff->getPath());
        if (node) {
            impl->undone--;

            Logs::debug("Definition") << "Node redo : " << node->getPath() << endl;
            node->applyDiff(diff);
            publishToWatchers(node, diff);
        } else {
            Logs::error("Definition") << "Can't find node to redo diff : " << diff->getPath() << endl;
        }
    }
}

void DiffManager::publishToWatchers(const DefinitionNode *node, const DefinitionDiff *diff) {
    impl->lock.lock();

    const DefinitionNode *cnode = node;
    do {
        for (auto watcher : impl->watchers[cnode]) {
            watcher->nodeChanged(node, diff, cnode);
        }
        // TODO Parent node signaling should be aggregated (to not receive many nodeChanged calls)
        cnode = cnode->getParent();
    } while (cnode);

    impl->lock.unlock();
}
