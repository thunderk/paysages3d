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

    int started;
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

    result->started = 0;
    result->last_segment.renderer = renderer;
    result->last_segment.layer = layer;

    result->next_action.order = CLOUD_WALKING_CONTINUE;

    return result;
}

void cloudsDeleteWalker(CloudsWalker* walker)
{
    free(walker);
}

void cloudsWalkerSetStepSize(CloudsWalker* walker, double step)
{
    if (step > 0.0)
    {
        walker->step_size = step;
    }
    else
    {
        /* TODO Automatic settings (using rendering quality and cloud feature size) */
        walker->step_size = 1.0;
    }
}

static void _getPoint(CloudsWalker* walker, double cursor, CloudWalkerPoint* out_point)
{
    out_point->distance_from_start = cursor;
    out_point->location = v3Add(walker->start, v3Scale(walker->diff, out_point->distance_from_start / walker->max_length));

    Renderer* renderer = walker->last_segment.renderer;
    CloudsLayerDefinition* layer = walker->last_segment.layer;
    out_point->global_density = renderer->clouds->getLayerDensity(renderer, layer, out_point->location);
}

static void _refineSegment(CloudsWalker* walker, double start_cursor, double start_density, double end_cursor, double end_density, double precision, CloudWalkerPoint* result)
{
    CloudWalkerPoint middle;

    _getPoint(walker, (start_cursor + end_cursor) / 2.0, &middle);

    if (start_density == 0.0)
    {
        /* Looking for entry */
        if (middle.distance_from_start - start_cursor < precision)
        {
            *result = middle;
        }
        else if (middle.global_density == 0.0)
        {
            _refineSegment(walker, middle.distance_from_start, middle.global_density, end_cursor, end_density, precision, result);
        }
        else
        {
            _refineSegment(walker, start_cursor, start_density, middle.distance_from_start, middle.global_density, precision, result);
        }
    }
    else
    {
        /* Looking for exit */
        if (end_cursor - middle.distance_from_start < precision)
        {
            *result = middle;
        }
        else if (middle.global_density == 0.0)
        {
            _refineSegment(walker, start_cursor, start_density, middle.distance_from_start, middle.global_density, precision, result);
        }
        else
        {
            _refineSegment(walker, middle.distance_from_start, middle.global_density, end_cursor, end_density, precision, result);
        }
    }
}

int cloudsWalkerPerformStep(CloudsWalker* walker)
{
    if (!walker->started)
    {
        _getPoint(walker, 0.0, &walker->last_segment.end);
        walker->started = 1;
    }

    if (walker->next_action.order == CLOUD_WALKING_STOP || walker->cursor >= walker->max_length)
    {
        walker->next_action.order = CLOUD_WALKING_STOP;
        return 0;
    }
    else if (walker->next_action.order == CLOUD_WALKING_CONTINUE)
    {
        /* TODO Limit to end */
        walker->last_segment.start = walker->last_segment.end;

        walker->cursor += walker->step_size;

        _getPoint(walker, walker->cursor, &walker->last_segment.end);
        walker->last_segment.length = walker->step_size;
        walker->last_segment.refined = 0;

        return 1;
    }
    else if (walker->next_action.order == CLOUD_WALKING_REFINE)
    {
        /* Refine segment with dichotomy */
        _refineSegment(walker,
                       walker->last_segment.start.distance_from_start,
                       walker->last_segment.start.global_density,
                       walker->last_segment.end.distance_from_start,
                       walker->last_segment.end.global_density,
                       walker->next_action.precision,
                       (walker->last_segment.start.global_density == 0.0) ? (&walker->last_segment.start) : (&walker->last_segment.end));
        walker->last_segment.length = walker->last_segment.end.distance_from_start - walker->last_segment.start.distance_from_start;
        walker->last_segment.refined = 1;

        walker->next_action.order = CLOUD_WALKING_CONTINUE;

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
    walker->last_segment.data = data;
    while (cloudsWalkerPerformStep(walker))
    {
        callback(walker);
    }
}
