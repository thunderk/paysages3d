#ifndef _PAYSAGES_CLOUDS_WALKING_H_
#define _PAYSAGES_CLOUDS_WALKING_H_

#include "public.h"
#include "../tools/euclid.h"

/**
 * Functions to walk through a cloud layer.
 */

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
    Vector3 enter;
    Vector3 exit;
    double length;
} CloudPrimarySegment;

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
 * @param out_segments Allocated space to fill found segments
 * @return Number of segments found
 */
int cloudsGetLayerPrimarySegments(Renderer* renderer, CloudsLayerDefinition* layer, Vector3 start, Vector3 end, int max_segments, CloudPrimarySegment* out_segments);

#ifdef __cplusplus
}
#endif

#endif
