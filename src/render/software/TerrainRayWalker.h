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
class SOFTWARESHARED_EXPORT TerrainRayWalker {
  public:
    typedef struct {
        Vector3 hit_location; // Location of the hit
        double escape_angle;  // Angle used to shift the ray to escape the terrain (0.0 if no escape was possible)
    } TerrainHitResult;

  public:
    TerrainRayWalker(SoftwareRenderer *renderer);

    /**
     * Set the walker quality.
     *
     * @param displacement_safety Safety factor (around 1.0) to detect when displacement textures need to be applied
     * @param minimal_step Minimal length of a walking step
     * @param maximal_step Maximal length of a walking step
     * @param step_factor Precision factor of steps, depending on terrain proximity
     * @param max_distance Maximal distance allowed to travel before considering an escape
     * @param escape_step Angle step when allowing an escape angle
     */
    void setQuality(double displacement_safety, double minimal_step, double maximal_step, double step_factor,
                    double max_distance, double escape_step);
    void setQuality(double factor);

    /*!
     * \brief Update the walker internal data, from the renderer and scenery.
     */
    void update();

    /*!
     * \brief Start the walking process to find intersection
     *
     * \param start Point of origin of the ray
     * \param direction Ray direction (normalized vector)
     * \param escape_angle Maximal angle allowed to escape the terrain on hit (mainly for shadows computing)
     * \param result Object to store the results info
     * \return true if there was a hit
     */
    bool startWalking(const Vector3 &start, Vector3 direction, double escape_angle, TerrainHitResult &result);

  private:
    SoftwareRenderer *renderer;
    double ymin;
    double ymax;
    double displacement_base;

    // Quality control
    double displacement_safety;
    double minimal_step;
    double maximal_step;
    double step_factor;
    double max_distance;
    double escape_step;
};
}
}

#endif // TERRAINRAYWALKER_H
