#ifndef DEFINITIONNODE_H
#define DEFINITIONNODE_H

#include "definition_global.h"

#include <string>
#include <vector>

namespace paysages {
namespace definition {

/**
 * Base class for all nodes of the definition tree.
 */
class DEFINITIONSHARED_EXPORT DefinitionNode {
  public:
    DefinitionNode(DefinitionNode *parent, const string &name, const string &type_name = "");
    virtual ~DefinitionNode();

    virtual void save(PackStream *stream) const;
    virtual void load(PackStream *stream);

    virtual void copy(DefinitionNode *destination) const;
    virtual void validate();

    inline const string &getName() const {
        return name;
    }
    virtual void setName(const string &name);

    inline const string &getTypeName() const {
        return type_name;
    }

    virtual const Scenery *getScenery() const;

    inline const DefinitionNode *getParent() const {
        return parent;
    }
    inline const DefinitionNode *getRoot() const {
        return root;
    }
    inline DiffManager *getDiffManager() const {
        return diffs;
    }
    inline unsigned long getChildrenCount() const {
        return children.size();
    }

    /**
     * Return a string representation of the tree (mainly for debugging purposes).
     */
    virtual string toString(int indent = 0) const;

    /**
     * Return the path to this node, using '/' delimited syntax, with the first '/' being the root node.
     */
    string getPath() const;

    /**
     * Find a node in this tree, by its path (as returned by getPath).
     *
     * Return NULL if the path does not exists.
     */
    DefinitionNode *findByPath(const string &path) const;

    /**
     * Apply a diff to the internal value of this node.
     *
     * All internal node modifications should be done using this method, to be reversible.
     *
     * If 'backward' is true, the diff will be reversed, instead of applied.
     *
     * Return true if the diff could be applied.
     */
    virtual bool applyDiff(const DefinitionDiff *diff, bool backward = false);

    /**
     * Fill a diff array to be applied to initialize a proper state for a watcher.
     *
     * This method should be overridden by subclasses.
     */
    virtual void generateInitDiffs(vector<const DefinitionDiff *> *diffs) const;

    /**
     * Add a watcher over this node.
     *
     * The watcher will receive DefinitionDiff objects when this node changes.
     *
     * If 'init_diff' is set to true, a first diff (or several) will be be pushed immediately to initialize the state.
     */
    void addWatcher(DefinitionWatcher *watcher, bool init_diff = false);

    /**
     * Get the current number of watchers.
     */
    unsigned long getWatcherCount() const;

  protected:
    void addChild(DefinitionNode *child);
    void removeChild(DefinitionNode *child);
    virtual DefinitionNode *findChildByName(const string &name) const;

    /**
     * Get the size in bytes this child will consume when serialized to a stream.
     *
     * Return -1 if it can't be known. In this case, the saving will be done in a temporary
     * stream to know the exact size, which will not be very efficient.
     */
    int getStreamSize() const;

    /**
     * Add a diff to the DiffManager of this definition tree, for the current node.
     *
     * The manager will take immediate ownership of the diff, handling its freeing.
     *
     * The manager will decide if the diff should be committed and will call 'applyDiff' if needed.
     */
    void addDiff(const DefinitionDiff *diff);

  private:
    DefinitionNode *parent;
    DefinitionNode *root;
    DiffManager *diffs;
    string type_name;
    string name;
    vector<DefinitionNode *> children;
};
}
}

#endif // DEFINITIONNODE_H
