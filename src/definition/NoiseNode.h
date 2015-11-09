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
    NoiseNode(DefinitionNode *parent);
    virtual ~NoiseNode();

    inline const NoiseGenerator *getGenerator() {
        return noise;
    }

    /**
     * Set the number of levels to use in the noise generator.
     */
    void setLevels(int levels, double min_value = -1.0, double max_value = 1.0);

  protected:
    virtual void save(PackStream *stream) const override;
    virtual void load(PackStream *stream) override;
    virtual void copy(DefinitionNode *destination) const override;
    virtual void validate() override;

  private:
    NoiseGenerator *noise;
};
}
}

#endif // NOISENODE_H
