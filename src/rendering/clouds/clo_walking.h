#ifndef _PAYSAGES_CLOUDS_WALKING_H_
#define _PAYSAGES_CLOUDS_WALKING_H_

#include "public.h"
#include "../tools/euclid.h"

/**
 * Functions to walk through a cloud layer (sampling).
 */

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
    Vector3 entry_point;
    Vector3 exit_point;
    double length;
} CloudPrimarySegment;

typedef struct
{
    /** Distance factor of the control point from the segment start */
    double distance;
    /** Location of the control point */
    Vector3 location;
    /** Global density at the control point (no edge noise applied) */
    double global_density;
    /** Particle dentisy at the control point, using edge noise */
    double particle_density;
    /** Estimated distance to nearest cloud exit */
    double nearest_exit_distance;
} CloudSecondaryControlPoint;

/**
 * Optimize the search limits in a layer.
 *
 * @param layer The cloud layer
 * @param start Start of the search to optimize
 * @param end End of the search to optimize
 * @return 0 if the search is useless
 */
int cloudsOptimizeWalkingBounds(CloudsLayerDefinition* layer, Vector3* start, Vector3* end);

/**
 * Go through the cloud layer to find segments (parts of the lookup that are likely to contain cloud).
 *
 * @param renderer The renderer environment
 * @param layer The cloud layer
 * @param start Start position of the lookup
 * @param end End position of the lookup
 * @param max_segments Maximum number of segments to collect
 * @param out_segments Allocated space to fill found segments (must be at least 'max_segments' long)
 * @return Number of segments found
 */
int cloudsGetLayerPrimarySegments(Renderer* renderer, CloudsLayerDefinition* layer, Vector3 start, Vector3 end, int max_segments, CloudPrimarySegment* out_segments);

/**
 * Sample a primary segment with refined details, collecting material interaction.
 *
 * @param renderer The renderer environment
 * @param layer The cloud layer
 * @param segment The primary segment to sample
 * @param max_control_points Maximum number of control points to sample
 * @param out_control_points Allocated space to fill with secondary control points (must be at least 'max_control_points' long)
 * @return Number of control points sampled
 */
int cloudsGetLayerSecondarySampling(Renderer* renderer, CloudsLayerDefinition* layer, CloudPrimarySegment* segment, int max_control_points, CloudSecondaryControlPoint* out_control_points);

#ifdef __cplusplus
}
#endif

#endif
