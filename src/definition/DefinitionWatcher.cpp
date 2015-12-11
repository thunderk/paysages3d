#include "DefinitionWatcher.h"

#include "DefinitionNode.h"
#include "Logs.h"

DefinitionWatcher::DefinitionWatcher() {
}

void DefinitionWatcher::startWatching(const DefinitionNode *root, const string &path, bool init_diff) {
    DefinitionNode *node = root->findByPath(path);
    if (node) {
        node->addWatcher(this, init_diff);
    } else {
        Logs::warning() << "[Definition] Node not found for watching : " << path << endl;
    }
}
