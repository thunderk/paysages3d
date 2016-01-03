#ifndef NOISENODE_H
#define NOISENODE_H

#include "definition_global.h"

#include "DefinitionNode.h"

namespace paysages {
namespace definition {

/**
 * Definition node with noise parameters.
 */
class DEFINITIONSHARED_EXPORT NoiseNode : public DefinitionNode {
  public:
    NoiseNode(DefinitionNode *parent, const string &name = "noise");
    virtual ~NoiseNode();

    inline const FractalNoise *getGenerator() {
        return noise;
    }

  protected:
    virtual void save(PackStream *stream) const override;
    virtual void load(PackStream *stream) override;
    virtual void copy(DefinitionNode *destination) const override;
    virtual string toString(int indent) const override;

  private:
    FractalNoise *noise;
};
}
}

#endif // NOISENODE_H
