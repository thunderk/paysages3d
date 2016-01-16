#include "DefinitionWatcher.h"

#include "IntDiff.h"
#include "FloatDiff.h"
#include "DefinitionNode.h"
#include "Logs.h"

DefinitionWatcher::DefinitionWatcher() {
}

DefinitionWatcher::~DefinitionWatcher() {
    // FIXME watcher is not removed from the diff manager !
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

void DefinitionWatcher::startWatching(const DefinitionNode *root, const string &path, bool init_diff) {
    DefinitionNode *node = root->findByPath(path);
    if (node) {
        node->addWatcher(this, init_diff);
    } else {
        Logs::warning("Definition") << "Node not found for watching : " << path << endl;
    }
}
