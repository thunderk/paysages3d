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

/*
 * Private structure for the walker.
 */
struct CloudsWalker
{
    Vector3 start;
    Vector3 end;
    Vector3 diff;

    double cursor;
    double max_length;
    double step_size;

    CloudWalkerStepInfo last_segment;

    CloudWalkingNextAction next_action;
};


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
    CloudsWalker* result;

    result = (CloudsWalker*)malloc(sizeof (CloudsWalker));

    result->start = start;
    result->end = end;
    result->diff = v3Sub(end, start);
    result->max_length = v3Norm(result->diff);
    result->cursor = 0.0;
    result->step_size = 1.0;

    result->last_segment.renderer = renderer;
    result->last_segment.layer = layer;
    result->last_segment.walked_distance = 0.0;
    result->last_segment.end = start;

    result->next_action.order = CLOUD_WALKING_CONTINUE;

    return result;
}

void cloudsDeleteWalker(CloudsWalker* walker)
{
    free(walker);
}

void cloudsSetStepSize(CloudsWalker* walker, double step)
{
    /* TODO Negative step => automatic */
    walker->step_size = step;
}

int cloudsWalkerPerformStep(CloudsWalker* walker)
{
    if (walker->next_action.order == CLOUD_WALKING_STOP || walker->cursor >= walker->max_length)
    {
        walker->next_action.order = CLOUD_WALKING_STOP;
        return 0;
    }
    else if (walker->next_action.order == CLOUD_WALKING_CONTINUE)
    {
        /* TODO Limit to end */
        walker->last_segment.start = walker->last_segment.end;
        walker->last_segment.walked_distance = walker->cursor;

        walker->cursor += walker->step_size;

        walker->last_segment.end = v3Add(walker->start, v3Scale(walker->diff, walker->cursor / walker->max_length));
        walker->last_segment.length = walker->step_size;

        return 1;
    }
    else
    {
        /* TODO */
        return 0;
    }
}

void cloudsWalkerOrderStop(CloudsWalker* walker)
{
    walker->next_action.order = CLOUD_WALKING_STOP;
}

void cloudsWalkerOrderRefine(CloudsWalker* walker, double precision)
{
    walker->next_action.order = CLOUD_WALKING_REFINE;
    walker->next_action.precision = precision;
}

void cloudsWalkerOrderSubdivide(CloudsWalker* walker, double max_segments)
{
    walker->next_action.order = CLOUD_WALKING_SUBDIVIDE;
    walker->next_action.max_segments = max_segments;
}

CloudWalkerStepInfo* cloudsWalkerGetLastSegment(CloudsWalker* walker)
{
    return &walker->last_segment;
}

void cloudsStartWalking(CloudsWalker* walker, FuncCloudsWalkingCallback callback, void* data)
{
    while (cloudsWalkerPerformStep(walker))
    {
        callback(walker);
    }
}
