#include "DiffManager.h"

#include "DefinitionNode.h"
#include "DefinitionDiff.h"

DiffManager::DiffManager(DefinitionNode *tree):
    tree(tree)
{
    undone = 0;
}

void DiffManager::addWatcher(const DefinitionNode *node, DefinitionWatcher *watcher)
{
    watchers[node].push_back(watcher);
}

void DiffManager::addDiff(DefinitionNode *node, const DefinitionDiff *diff)
{
    diffs.push_back(diff);

    // TODO Delayed commit (with merge of consecutive diffs)
    node->applyDiff(diff);

    for (auto &watcher: watchers[node])
    {
        // TODO
    }
}

void DiffManager::undo()
{
    if (undone <= (int)diffs.size())
    {
        undone++;
        const DefinitionDiff *diff = diffs[diffs.size() - undone];

        // Obtain the node by path and reverse apply diff on it
        DefinitionNode *node = tree->findByPath(diff->getPath());
        node->applyDiff(diff, true);
    }
}

void DiffManager::redo()
{
    if (undone > 0)
    {
        const DefinitionDiff *diff = diffs[diffs.size() - undone];
        undone--;

        // Obtain the node by path and re-apply diff on it
        DefinitionNode *node = tree->findByPath(diff->getPath());
        node->applyDiff(diff);
    }
}

