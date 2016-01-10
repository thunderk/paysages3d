#ifndef TEXTURESRENDERER_H
#define TEXTURESRENDERER_H

#include "software_global.h"

#include <vector>

namespace paysages {
namespace software {

class SOFTWARESHARED_EXPORT TexturesRenderer {
  public:
    TexturesRenderer();
    virtual ~TexturesRenderer();

    /**
     * Set the quality parameters.
     *
     * "normal5" can be set to true to use balanced 5 points instead of unbalanced 3 points for normal computations.
     */
    void setQuality(bool normal5);

    /**
     * Set an automated quality factor.
     */
    void setQualityFactor(double factor);

    /**
     * Get the maximal displacement offset all combined textures can make.
     */
    double getMaximalDisplacement(TexturesDefinition *textures) const;

    /**
     * Get the fully displaced terrain location (applying all textures).
     */
    Vector3 displaceTerrain(const TexturesDefinition *textures, const Vector3 &location, const Vector3 &normal) const;

    /**
     * Get the presence of each texture layer at a given terrain location.
     */
    vector<double> getLayersPresence(const TexturesDefinition *textures, const Vector3 &location,
                                     const Vector3 &normal) const;

    /**
     * Get the displaced location of each texture layer at a given terrain location.
     *
     * 'presence' is the result of 'getLayersPresence'.
     */
    vector<Vector3> getLayersDisplacement(const TexturesDefinition *textures, const Vector3 &location,
                                          const Vector3 &normal, const vector<double> &presence) const;

    /**
     * Get the final lighted texture composition.
     *
     * 'presence' is the result of 'getLayersPresence'.
     * 'location' is the result of 'getLayersDisplacement'.
     * 'normal' is the normal vector (taking only displacement into account, not detail) at each texture's 'location'.
     * 'precision' is the level of detail needed for the composition (minimal height of the detail noise).
     */
    Color getFinalComposition(const TexturesDefinition *textures, LightingManager *lighting,
                              const vector<double> &presence, const vector<Vector3> &location,
                              const vector<Vector3> &normal, double precision, const Vector3 &eye) const;

  private:
    bool quality_normal5;
};
}
}

#endif // TEXTURESRENDERER_H
