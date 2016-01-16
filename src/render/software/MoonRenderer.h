#ifndef MOONRENDERER_H
#define MOONRENDERER_H

#include "software_global.h"

#include "DefinitionWatcher.h"
#include "LightSource.h"

#include <memory>

namespace paysages {
namespace software {

/**
 * Moon renderer.
 */
class SOFTWARESHARED_EXPORT MoonRenderer : public DefinitionWatcher, public LightSource {
  public:
    MoonRenderer(CelestialBodyDefinition *moon_node);
    virtual ~MoonRenderer();

    virtual void nodeChanged(const DefinitionNode *node, const DefinitionDiff *diff, const DefinitionNode *parent) override;
    virtual bool getLightsAt(vector<LightComponent> &result, const Vector3 &location) const override;

    /**
     * Get the final moon color, given a looking ray.
     *
     * The result will be lighted with *lighting*.
     */
    Color getColor(const Vector3 &eye, const Vector3 &direction, LightingManager *lighting) const;

  private:
    class pimpl;
    unique_ptr<pimpl> impl;

};
}
}

#endif // MOONRENDERER_H
