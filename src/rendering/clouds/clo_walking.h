#ifndef _PAYSAGES_CLOUDS_WALKING_H_
#define _PAYSAGES_CLOUDS_WALKING_H_

#include "public.h"
#include "../tools/euclid.h"

/**
 * Functions to walk through a cloud layer.
 */

typedef struct
{
    double distance_from_start;
    Vector3 location;
    double global_density;
    double local_density;
} CloudWalkerPoint;

/**
 * Information on a segment yielded by walking.
 */
typedef struct
{
    Renderer* renderer;
    CloudLayerDefinition* layer;

    CloudWalkerPoint start;
    CloudWalkerPoint end;
    double length;

    int refined;
    int subdivided;

    void* data;
} CloudWalkerStepInfo;

typedef struct CloudsWalker CloudsWalker;

typedef void (*FuncCloudsWalkingCallback)(CloudsWalker* walker);

/**
 * Optimize the search limits in a layer.
 *
 * @param layer The cloud layer
 * @param start Start of the search to optimize
 * @param end End of the search to optimize
 * @return 0 if the search is useless
 */
RENDERINGSHARED_EXPORT int cloudsOptimizeWalkingBounds(CloudLayerDefinition* layer, Vector3* start, Vector3* end);

/**
 * Create a cloud walker.
 *
 * For better performance, the segment should by optimized using cloudsOptimizeWalkingBounds.
 * @param renderer Renderer context
 * @param layer The cloud layer to traverse
 * @param start Start of the walk
 * @param end End of the walk
 */
RENDERINGSHARED_EXPORT CloudsWalker* cloudsCreateWalker(Renderer* renderer, CloudLayerDefinition* layer, Vector3 start, Vector3 end);

/**
 * Delete a cloud walker.
 *
 * @param walker The walker to free
 */
RENDERINGSHARED_EXPORT void cloudsDeleteWalker(CloudsWalker* walker);

/**
 * Define the segment size for next steps.
 *
 * @param walker The walker to configure
 * @param step The step length, negative for automatic
 */
RENDERINGSHARED_EXPORT void cloudsWalkerSetStepSize(CloudsWalker* walker, double step);

/**
 * Set the void skipping mode.
 *
 * @param walker The walker to configure
 * @param enabled 1 to enable the void skipping, 0 to disable
 */
RENDERINGSHARED_EXPORT void cloudsWalkerSetVoidSkipping(CloudsWalker* walker, int enabled);

/**
 * Toggle the local density computing.
 *
 * When this option is set, the CloudWalkerStepInfo will contain information about local density.
 * The automatic setting will set to 1 on subdivided steps, and 0 elsewhere.
 * @param walker The walker to configure
 * @param enabled 1 to enable local density, 0 to disable it, -1 for automatic setting.
 */
RENDERINGSHARED_EXPORT void cloudsWalkerToggleLocalDensity(CloudsWalker* walker, int enabled);

/**
 * Perform a single step.
 *
 * @param walker The walker to use
 * @return 1 to continue the loop, 0 to stop
 */
RENDERINGSHARED_EXPORT int cloudsWalkerPerformStep(CloudsWalker* walker);

/**
 * Order the walker to stop.
 *
 * @param walker The walker to use
 */
RENDERINGSHARED_EXPORT void cloudsWalkerOrderStop(CloudsWalker* walker);

/**
 * Order the walker to refine the search for cloud entry or exit.
 *
 * The refinement will next yield a shorter version of the segment, containing only the cloud-inside portion, with a
 * tolerance fixed by precision. For an entry point, this will discard the part before cloud entry. For en exit point,
 * the portion after this point will be part of the next step, as normal walking resumes.
 * @param walker The walker to use
 * @param precision Precision wanted for the refinement
 */
RENDERINGSHARED_EXPORT void cloudsWalkerOrderRefine(CloudsWalker* walker, double precision);

/**
 * Order the walker to subdivide the previous segment in smaller segments.
 *
 * Next steps will yield subdivided segments. Once subdivided segments have been processed, normal walking
 * will resume automatically.
 * @param walker The walker to use
 * @param max_segments Maximal number of segments
 */
RENDERINGSHARED_EXPORT void cloudsWalkerOrderSubdivide(CloudsWalker* walker, double max_segments);

/**
 * Get the last segment information.
 *
 * @param walker The walker to use
 */
RENDERINGSHARED_EXPORT CloudWalkerStepInfo* cloudsWalkerGetLastSegment(CloudsWalker* walker);

/**
 * Start walking automatically through a segment.
 *
 * The callback will be called with each segment found, giving info and asking for desired alteration on walking.
 * @param walker The walker to use
 * @param callback Callback to be called with each found segment
 * @param data User data that will be passed back in the callback
 */
RENDERINGSHARED_EXPORT void cloudsStartWalking(CloudsWalker* walker, FuncCloudsWalkingCallback callback, void* data);

#endif
