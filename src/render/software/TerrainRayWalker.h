#ifndef TERRAINRAYWALKER_H
#define TERRAINRAYWALKER_H

#include "software_global.h"

#include "Vector3.h"

namespace paysages {
namespace software {

/*!
 * \brief Ray walker to find intersections with terrain.
 *
 * This walker can be used to find a hard intersection between
 * a ray and the terrain (e.g. for raytracing), or a soft intersection
 * (e.g. for shadows).
 */
class SOFTWARESHARED_EXPORT TerrainRayWalker
{
public:
    typedef struct {
        Vector3 hit_location; // Location of the hit
        double escape_length; // Length walked before terrain was escaped after hit (0.0 if no escape)
    } TerrainHitResult;

public:
    TerrainRayWalker(SoftwareRenderer* renderer);

    /*!
     * \brief Update the walker internal data, from the renderer and scenery.
     */
    void update();

    /*!
     * \brief Start the walking process to find intersection
     *
     * \param start Point of origin of the ray
     * \param direction Ray direction (normalized vector)
     * \param escaping_factor Factor used to escape the terrain on hit (mainly for shadows computing)
     * \param max_length Maximum length to walk before considering no hit
     * \param result Object to store the results info
     * \return true if there was a hit
     */
    bool startWalking(Vector3 start, const Vector3 &direction, double escaping_factor, double max_length, TerrainHitResult &result);

private:
    SoftwareRenderer* renderer;
    double ymin;
    double ymax;
    double ydispmin;
    double ydispmax;
    double minstep;
    double maxstep;
};

}
}

#endif // TERRAINRAYWALKER_H
