#include "clo_walking.h"

#include "../renderer.h"

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
    CloudWalkingOrder order;
    double precision;
    int max_segments;
} CloudWalkingNextAction;


int cloudsOptimizeWalkingBounds(CloudsLayerDefinition* layer, Vector3* start, Vector3* end)
{
    Vector3 diff;

    if (start->y > layer->lower_altitude + layer->thickness)
    {
        if (end->y >= layer->lower_altitude + layer->thickness)
        {
            return 0;
        }
        else
        {
            diff = v3Sub(*end, *start);
            *start = v3Add(*start, v3Scale(diff, (layer->lower_altitude + layer->thickness - start->y) / diff.y));
            if (end->y < layer->lower_altitude)
            {
                *end = v3Add(*end, v3Scale(diff, (layer->lower_altitude - end->y) / diff.y));
            }
        }
    }
    else if (start->y < layer->lower_altitude)
    {
        if (end->y <= layer->lower_altitude)
        {
            return 0;
        }
        else
        {
            diff = v3Sub(*end, *start);
            *start = v3Add(*start, v3Scale(diff, (layer->lower_altitude - start->y) / diff.y));
            if (end->y >= layer->lower_altitude + layer->thickness)
            {
                *end = v3Add(*end, v3Scale(diff, (layer->lower_altitude + layer->thickness - end->y) / diff.y));
            }
        }
    }
    else /* start is inside layer */
    {
        diff = v3Sub(*end, *start);
        if (end->y > layer->lower_altitude + layer->thickness)
        {
            *end = v3Add(*start, v3Scale(diff, (layer->lower_altitude + layer->thickness - start->y) / diff.y));
        }
        else if (end->y < layer->lower_altitude)
        {
            *end = v3Add(*start, v3Scale(diff, (layer->lower_altitude - start->y) / diff.y));
        }
    }

    return 1;
}

CloudsWalker* cloudsCreateWalker(Renderer* renderer, CloudsLayerDefinition* layer, Vector3 start, Vector3 end)
{
}

void cloudsDeleteWalker(CloudsWalker* walker)
{
}

void cloudsWalkerPerformStep(CloudsWalker* walker)
{
}

void cloudsWalkerOrderStop(CloudsWalker* walker)
{
}

void cloudsWalkerOrderRefine(CloudsWalker* walker, double precision)
{
}

void cloudsWalkerOrderSubdivide(CloudsWalker* walker, double max_segments)
{
}

CloudWalkerStepInfo* cloudsWalkerGetLastSegment(CloudsWalker* walker)
{
}

void cloudsStartWalking(CloudsWalker* walker, FuncCloudsWalkingCallback callback, void* data)
{
}
