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

/**
 * Information on a segment yielded by walking.
 */
typedef struct
{
    Renderer* renderer;
    CloudsLayerDefinition* layer;

    double walked_distance;
    Vector3 start;
    Vector3 end;
    double length;

    int refined;
    int subdivision_level;
    double precision_asked;

    void* data;
} CloudWalkingInfo;

/**
 * Control of the next walking order.
 */
typedef enum
{
    CLOUD_WALKING_CONTINUE,
    CLOUD_WALKING_STOP,
    CLOUD_WALKING_REFINE,
    CLOUD_WALKING_SUBDIVIDE
} CloudWalkingOrder;

/**
 * Additional info for walking orders.
 */
typedef struct
{
    double precision;
    int max_segments;
} CloudWalkingOrderInfo;

typedef CloudWalkingOrder(*FuncCloudSegmentCallback)(CloudWalkingInfo* segment, CloudWalkingOrderInfo* order);

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
 * Start walking through a segment.
 *
 * For better performance, the segment should by optimized using cloudsOptimizeWalkingBounds.
 * The callback will be called with each segment found, giving info and asking for desired alteration on walking.
 * @param renderer The renderer environment
 * @param layer The cloud layer
 * @param start Start position of the lookup, already optimized
 * @param end End position of the lookup, already optimized
 * @param callback Callback to be called with each found segment
 * @param data User data that will be passed back in the callback
 */
void cloudsStartWalking(Renderer* renderer, CloudsLayerDefinition* layer, Vector3 start, Vector3 end, FuncCloudSegmentCallback callback, void* data);

#ifdef __cplusplus
}
#endif

#endif
