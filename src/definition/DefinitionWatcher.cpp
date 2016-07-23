#include "DefinitionWatcher.h"

#include "DefinitionNode.h"
#include "DiffManager.h"
#include "FloatDiff.h"
#include "IntDiff.h"
#include "Logs.h"

DefinitionWatcher::DefinitionWatcher(const string &name) : name(name) {
}

DefinitionWatcher::~DefinitionWatcher() {
    if (registered_to.size() > 0) {
        Logs::error("Definition")
            << "Watcher still registered at destruction, forcing potentially harmful unregister : " << name << endl;
        unregister();
    }
}

void DefinitionWatcher::unregister() {
    set<DiffManager *> copy_registered_to = registered_to;
    registered_to.clear();
    for (auto diff_manager : copy_registered_to) {
        diff_manager->removeWatcher(this);
    }
}

void DefinitionWatcher::nodeChanged(const DefinitionNode *node, const DefinitionDiff *diff, const DefinitionNode *) {
    string type_name = node->getTypeName();

    if (type_name == "int") {
        auto int_diff = static_cast<const IntDiff *>(diff);
        intNodeChanged(node->getPath(), int_diff->getNewValue(), int_diff->getOldValue());
    } else if (type_name == "float") {
        auto float_diff = static_cast<const FloatDiff *>(diff);
        floatNodeChanged(node->getPath(), float_diff->getNewValue(), float_diff->getOldValue());
    }
}

void DefinitionWatcher::intNodeChanged(const string &, int, int) {
}

void DefinitionWatcher::floatNodeChanged(const string &, double, double) {
}

void DefinitionWatcher::startWatchingPath(const DefinitionNode *root, const string &path, bool init_diff) {
    DefinitionNode *node = root->findByPath(path);
    if (node) {
        startWatching(node, init_diff);
    } else {
        Logs::warning("Definition") << "Node not found for watching : " << path << endl;
    }
}

void DefinitionWatcher::startWatching(const DefinitionNode *node, bool init_diff) {
    if (auto diff_manager = node->getRoot()->getDiffManager()) {
        if (init_diff) {
            vector<const DefinitionDiff *> diffs;
            node->generateInitDiffs(&diffs);

            for (auto diff : diffs) {
                nodeChanged(node, diff, node);
                delete diff;
            }
        }
        diff_manager->addWatcher(node, this);
        registered_to.insert(diff_manager);
    }
}
