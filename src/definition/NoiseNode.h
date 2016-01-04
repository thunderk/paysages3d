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

    inline const FractalNoise *getGenerator() const {
        return noise;
    }

    /**
     * Randomize the noise (only the noise, not its configuration).
     */
    void randomize(RandomGenerator &random);

    /**
     * Set the noise configuration.
     */
    void setConfig(double scaling, double step_scaling = 0.5, double height = 1.0, double step_height = 0.5);

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
