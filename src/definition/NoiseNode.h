#pragma once

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
    void setConfig(double scaling, double height = 1.0, double step_scaling = 0.5, double step_height = 1.0);

    /**
     * Force the noise generator to use.
     *
     * This should only be needed in testing, and is not thread-safe.
     *
     * The node will take ownership of the generator and will take care of its destruction.
     */
    void forceSetGenerator(FractalNoise *noise);

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
