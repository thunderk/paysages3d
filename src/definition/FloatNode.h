#ifndef FLOATNODE_H
#define FLOATNODE_H

#include "definition_global.h"

#include "DefinitionNode.h"

namespace paysages {
namespace definition {

/**
 * Node with a single floating point numeric value, for the definition tree.
 */
class DEFINITIONSHARED_EXPORT FloatNode: public DefinitionNode
{
public:
    FloatNode(DefinitionNode* parent, const std::string &name, double value = 0.0);

    inline double getValue() const {return value;}

    virtual std::string toString(int indent) const override;
    virtual void save(PackStream* stream) const override;
    virtual void load(PackStream* stream) override;
    virtual void copy(DefinitionNode* destination) const override;
private:
    double value;
};

}
}

#endif // FLOATNODE_H
