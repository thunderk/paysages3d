#ifndef MATERIALNODE_H
#define MATERIALNODE_H

#include "definition_global.h"

#include "DefinitionNode.h"
#include <memory>

namespace paysages {
namespace definition {

/**
 * Raw surface material for the definition tree, with an optional bump mapping noise.
 */
class DEFINITIONSHARED_EXPORT MaterialNode : public DefinitionNode {
  public:
    MaterialNode(DefinitionNode *parent, const string &name);

    const SurfaceMaterial &getSurfaceMaterial() const;
    void setSurfaceMaterial(const SurfaceMaterial &material);
    void setSurfaceMaterial(const Color &color, double reflection, double shininess, double hardness=0.5, double ambient=0.5);

    bool hasBump() const;
    const FractalNoise *getBumpGenerator() const;

    inline const ColorNode *propDiffuse() const {
        return diffuse;
    }
    inline const FloatNode *propAmbient() const {
        return ambient;
    }
    inline const FloatNode *propHardness() const {
        return hardness;
    }
    inline const FloatNode *propReflection() const {
        return reflection;
    }
    inline const FloatNode *propShininess() const {
        return shininess;
    }
    inline NoiseNode *propBump() const {
        return bump;
    }

  protected:
    virtual void onChildChanged(int depth, const string &relpath) override;

  private:
    void updateMaterial();

  private:
    unique_ptr<SurfaceMaterial> material;
    ColorNode *diffuse;
    FloatNode *ambient;
    FloatNode *hardness;
    FloatNode *reflection;
    FloatNode *shininess;
    NoiseNode *bump;
};
}
}

#endif // MATERIALNODE_H
