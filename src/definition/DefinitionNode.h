#ifndef DEFINITIONNODE_H
#define DEFINITIONNODE_H

#include "definition_global.h"

namespace paysages {
namespace definition {

/**
 * Base class for all nodes of the definition tree.
 */
class DEFINITIONSHARED_EXPORT DefinitionNode
{
public:
    DefinitionNode(DefinitionNode* parent, const std::string &name);
    virtual ~DefinitionNode();

    virtual void save(PackStream* stream) const;
    virtual void load(PackStream* stream);

    virtual void copy(DefinitionNode* destination) const;
    virtual void validate();

    inline const std::string &getName() const {return name;}
    virtual void setName(const std::string &name);

    virtual Scenery* getScenery();

    inline const DefinitionNode* getParent() const {return parent;}
    inline const DefinitionNode* getRoot() const {return root;}
    inline int getChildrenCount() const {return children.size();}

    /**
     * Return a string representation of the tree (mainly for debugging purposes).
     */
    virtual std::string toString(int indent = 0) const;

protected:
    void addChild(DefinitionNode* child);
    void removeChild(DefinitionNode* child);

private:
    DefinitionNode* parent;
    DefinitionNode* root;
    std::string name;
    std::vector<DefinitionNode*> children;
};

}
}

#endif // DEFINITIONNODE_H
